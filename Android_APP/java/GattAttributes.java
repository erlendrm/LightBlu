package com.example.mads.lightblu;


import java.util.HashMap;

public class GattAttributes {
    private static HashMap<String, String> attributes = new HashMap();
    public static String LIGHT_CONTROL_SERVICE = "2f160001-e5dc-11e4-9cf7-0002a5d5c51b";
    public static String LIGHT_COMMAND = "2f160002-e5dc-11e4-9cf7-0002a5d5c51b";
    public static String LIGHT_STATUS = "2f160003-e5dc-11e4-9cf7-0002a5d5c51b";
    public static String CLIENT_CHARACTERISTIC_CONFIG = "00002902-0000-1000-8000-00805f9b34fb";

    static {
        //Services.
        attributes.put("2f160001-e5dc-11e4-9cf7-0002a5d5c51b", "Light Control Service");
        attributes.put(LIGHT_CONTROL_SERVICE, "Lyskontroll");

        //Characteristics.
        attributes.put(LIGHT_COMMAND, "Endre Lys");

        attributes.put(LIGHT_STATUS, "Lysstatus");
    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }
}
