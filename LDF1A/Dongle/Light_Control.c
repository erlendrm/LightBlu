////////////////////////////////////////////////
/*	Code for Light Control Service 						*/
/*	By: Erlend, E1534B "LightBlu" HiST 2015		*/
////////////////////////////////////////////////

#include "Light_Control.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"

#define BLE_LC_MAX_DATA_LEN		1		// Maximum length of data in bytes


// Function for handling the Connect event
static void on_connect (ble_lc_t * p_lc, ble_evt_t * p_ble_evt)
{
		p_lc->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

// Function for handling the Disconnec event
static void on_disconnect(ble_lc_t * p_lc, ble_evt_t * p_ble_evt)
{
		UNUSED_PARAMETER(p_ble_evt);
		p_lc->conn_handle = BLE_CONN_HANDLE_INVALID;
}

//Function for handling the Write event
static void on_write(ble_lc_t * p_lc, ble_evt_t * p_ble_evt)
{
	ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (
        (p_evt_write->handle == p_lc->light_status_handles.cccd_handle)
        &&
        (p_evt_write->len == 2)
       )
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
        {
            p_lc->is_notification_enabled = true;
        }
        else
        {
            p_lc->is_notification_enabled = false;
        }
				
    }
    else if (
             (p_evt_write->handle == p_lc->light_cmd_handles.value_handle)
             &&
             (p_lc->data_handler_cmd != NULL)
            )
    {
        p_lc->data_handler_cmd(p_lc, p_evt_write->data, p_evt_write->len);
    }
    else
    {
        // Do Nothing. This event is not relevant for this service.
    }
}


//Function for handling Read Authorization
static void on_read(ble_lc_t * p_lc, ble_evt_t * p_ble_evt)
{
		uint8_t	update_data;
		ble_gatts_rw_authorize_reply_params_t	 reply_params;
		
		update_data = p_lc->data_handler_status(); 
	
		memset(&reply_params, 0, sizeof(reply_params));
	
		reply_params.type								= BLE_GATTS_AUTHORIZE_TYPE_READ;
		reply_params.params.read.p_data = &update_data;
		reply_params.params.read.len		= sizeof(update_data);
		reply_params.params.read.offset	= 0;
		reply_params.params.read.update	= 1;
	
		sd_ble_gatts_rw_authorize_reply(p_lc->conn_handle, &reply_params);
}

void ble_lc_on_ble_event(ble_lc_t * p_lc, ble_evt_t * p_ble_evt)
{
	if ((p_lc == NULL) || (p_ble_evt == NULL))
  {
     return;
  }
	
	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			on_connect(p_lc, p_ble_evt);
			break;
		
		case BLE_GAP_EVT_DISCONNECTED:
			on_disconnect(p_lc, p_ble_evt);
			break;
		
		case BLE_GATTS_EVT_WRITE:
			on_write(p_lc, p_ble_evt);
			break;
		
		case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
			on_read(p_lc, p_ble_evt);
			break;
		
		default:
			// Nothing needed
			break;		
	}
}

// Function for adding the Light Status characteristic
static uint32_t light_status_char_add(ble_lc_t * p_lc, const ble_lc_init_t * p_lc_init, ble_uuid_t * ble_uuid)
{
		ble_gatts_char_md_t		char_md;
		ble_gatts_attr_md_t		cccd_md;
		ble_gatts_attr_t			attr_char_value;
		ble_gatts_attr_md_t		attr_md;
	
		memset(&cccd_md, 0, sizeof(cccd_md));
	
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
		
		cccd_md.vloc	= BLE_GATTS_VLOC_STACK;
	
		memset(&char_md, 0, sizeof(char_md));
	
		char_md.char_props.notify		= NULL;
		char_md.char_props.read			= 1;
		char_md.p_char_user_desc		= NULL;
		char_md.p_char_pf						= NULL;
		char_md.p_user_desc_md			= NULL;
		char_md.p_cccd_md						= &cccd_md;
		char_md.p_sccd_md						= NULL;
		
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
		
		attr_md.vloc				= BLE_GATTS_VLOC_STACK;
		attr_md.rd_auth			= 1;
		attr_md.wr_auth			= 0;
		attr_md.vlen				= 1;
		
		memset(&attr_char_value, 0, sizeof(attr_char_value));
		
		attr_char_value.p_uuid			= ble_uuid;
		attr_char_value.p_attr_md		= &attr_md;
		attr_char_value.init_len		= sizeof(uint8_t);
		attr_char_value.init_offs		= 0;
		attr_char_value.max_len			= BLE_LC_MAX_DATA_LEN;
		
		return sd_ble_gatts_characteristic_add(p_lc->service_handle,
																					 &char_md,
																					 &attr_char_value,
																					 &p_lc->light_status_handles);
}


// Function for adding the Light Command characteristic
static uint32_t light_cmd_char_add(ble_lc_t * p_lc, const ble_lc_init_t * p_lc_init, ble_uuid_t * ble_uuid)
{
		ble_gatts_char_md_t		char_md;
		ble_gatts_attr_t			attr_char_value;
		ble_gatts_attr_md_t		attr_md;
	
		memset(&char_md, 0, sizeof(char_md));
	
		char_md.char_props.write					= 1;
		char_md.p_char_user_desc					= NULL;
		char_md.p_char_pf									= NULL;
		char_md.p_user_desc_md						= NULL;
		char_md.p_cccd_md									= NULL;
		char_md.p_sccd_md									= NULL;
		
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
		
		attr_md.vloc				= BLE_GATTS_VLOC_STACK;
		attr_md.rd_auth			= 0;
		attr_md.wr_auth			= 0;
		attr_md.vlen				= 1;
		
		memset(&attr_char_value, 0, sizeof(attr_char_value));
		
		attr_char_value.p_uuid			= ble_uuid;
		attr_char_value.p_attr_md		= &attr_md;
		attr_char_value.init_len		= sizeof(uint8_t);
		attr_char_value.init_offs		= 0;
		attr_char_value.max_len			= BLE_LC_MAX_DATA_LEN;
		
		return sd_ble_gatts_characteristic_add(p_lc->service_handle,
																					 &char_md,
																					 &attr_char_value,
																					 &p_lc->light_cmd_handles);		
}


// Function for initializing the Light Control Service
uint32_t ble_lc_init(ble_lc_t * p_lc, const ble_lc_init_t * p_lc_init, ble_uuid_t * ble_uuid_service, ble_uuid_t * ble_uuid_char_status, ble_uuid_t * ble_uuid_char_cmd)
{
	uint32_t err_code;
	
	// Initialize service structure
	p_lc->conn_handle								= BLE_CONN_HANDLE_INVALID;
	p_lc->data_handler_cmd					= p_lc_init->data_handler_cmd;
	p_lc->data_handler_status				= p_lc_init->data_handler_status;
	p_lc->is_notification_enabled		= false;
	
	// Add the Light Control service
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
																			ble_uuid_service,
																			&p_lc->service_handle);
	if (err_code != NRF_SUCCESS)
  {
			return err_code;
  }
	
	// Add the Light Status characteristic
	err_code = light_status_char_add(p_lc, p_lc_init, ble_uuid_char_status);
	if (err_code != NRF_SUCCESS)
  {
			return err_code;
  }
	
	// Add the Light Command characteristic
	err_code = light_cmd_char_add(p_lc, p_lc_init, ble_uuid_char_cmd);
	if (err_code != NRF_SUCCESS)
  {
			return err_code;
  }
	
	return NRF_SUCCESS;	
}


// Function for updating Light Status
uint32_t ble_lc_light_status_update(ble_lc_t * p_lc, uint8_t * light_status, uint16_t length)
{
		ble_gatts_hvx_params_t		hvx_params;
	
		if (p_lc == NULL)
		{
				return NRF_ERROR_NULL;
		}
		
		if ((p_lc->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_lc->is_notification_enabled))
		{
				return NRF_ERROR_INVALID_STATE;
		}
		
		if (length > BLE_LC_MAX_DATA_LEN)
		{
				return NRF_ERROR_INVALID_PARAM;
		}
		
		memset(&hvx_params, 0, sizeof(hvx_params));
		
		hvx_params.handle		= p_lc->light_status_handles.value_handle;
		hvx_params.p_data		= light_status;
		hvx_params.p_len		= &length;
		hvx_params.type			= BLE_GATT_HVX_NOTIFICATION;
		
		return	sd_ble_gatts_hvx(p_lc->conn_handle, &hvx_params);
}
