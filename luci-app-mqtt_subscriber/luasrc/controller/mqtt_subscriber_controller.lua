module("luci.controller.mqtt_subscriber_controller", package.seeall)

function index()
    entry( { "admin", "services", "mqtt", "subscriber" }, cbi("mqtt_subscriber_model"), _("Subscriber"), 3).leaf = true
end