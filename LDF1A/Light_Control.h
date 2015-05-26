////////////////////////////////////////////////
/*	Include file for Light Control Service 		*/
/*	By: Erlend, E1534B "LightBlu" HiST 2015		*/
////////////////////////////////////////////////

#ifndef BLE_LC_H__
#define BLE_LC_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

extern uint8_t global_data;

// Forward declaration of the ble_lc_t type
typedef struct	ble_lc_s	ble_lc_t;

// Light Control Service event handler type
typedef void (*ble_lc_data_handler_t) (ble_lc_t * p_lc, uint8_t * p_data, uint16_t length);
typedef uint8_t	(*ble_lc_status_handler_t)	(void);

// Light Control Service init structure. This contains all options and data needed for initialization of the service
typedef struct
{
    ble_lc_data_handler_t data_handler_cmd;			// Event handler to be called for handling received commands
		ble_lc_status_handler_t data_handler_status;	// Event handler to be called for handling sending of status 
} ble_lc_init_t;


// Light Control Service structure. This contains various information about the service
struct ble_lc_s
{
    uint16_t                 	service_handle;          	// Handle of Light Control Service (as provided by the S110 SoftDevice)
    ble_gatts_char_handles_t 	light_status_handles;    	// Handles related to the Light Status characteristic (as provided by the S110 SoftDevice)
    ble_gatts_char_handles_t 	light_cmd_handles;       	// Handles related to the LIght Command characteristic (as provided by the S110 SoftDevice)
    uint16_t                 	conn_handle;             	// Handle of the current connection (as provided by the S110 SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection
    bool                     	is_notification_enabled; 	// Variable to indicate if the peer has enabled notification of the Light Status characteristic
    ble_lc_data_handler_t   	data_handler_cmd; 	      // Event handler to be called for handling received commands
		ble_lc_status_handler_t		data_handler_status;			// Event handler to be called for handling sending of status
};

// Function for initializing the Light Control Service
uint32_t	ble_lc_init(ble_lc_t * p_lc, const ble_lc_init_t * p_lc_init, ble_uuid_t * ble_uuid_service, ble_uuid_t * ble_uuid_char_status, ble_uuid_t * ble_uuid_char_cmd);

// Function for handling the Application's BLE Stack events
void ble_lc_on_ble_event(ble_lc_t * p_lc, ble_evt_t * p_ble_evt);

// Function for reading Light Status
uint32_t ble_lc_light_status_update(ble_lc_t * p_lc, uint8_t * light_status, uint16_t length);

#endif // BLE_LC_H__
