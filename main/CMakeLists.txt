# embed ca_cert certificate into binary
if(CONFIG_OTA_ENABLED)
    set(cert_pem ${project_dir}/server_certs/ca_cert.pem)
else()
    set(cert_pem "")
endif()

idf_component_register(SRCS "main.cpp"
                            "http.cpp"
                            "tasks.cpp"
                            "wifi.cpp"
                            "ble.cpp"
                            "dht11.cpp"
                            "bme680.cpp"
                            "ota.cpp"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ${cert_pem}
)
