# mqtt_subscriber

* section
  * option
    * explanation

Structure of the config file:

* mqtt_subscriber 'mqtt_subscriber_sct' (single)
  * enable
    * on | off flag
  * host
    * MQTT broker address
  * port
    * MQTT broker port
  * ca_file
    * CA file path
  * username
    * MQTT client username
  * password
    * MQTT client password
  * use_tls
    * TLS flag, 1 - enabled
* topic (multiple)
  * topic
    * topic name
  * qos
    * topic QOS
* event (multiple)
  * topic
    * topic name
  * parameter
    * variable name
  * type
    * variable type: 0 - integer, 1 - string
  * operator
    * 0 - ==, 1 - !=, 2 - >, 3 - <, 4 - >=, 5 - <=
  * expected_value
    * variable will be compared with this value (should be the same type as variable)
  * email
    * recipients email account name, SMTP configuration (System -> Administration -> Recipients -> Email Accounts)
  * receiver
    * Mail will be sent to this email address
  
  
