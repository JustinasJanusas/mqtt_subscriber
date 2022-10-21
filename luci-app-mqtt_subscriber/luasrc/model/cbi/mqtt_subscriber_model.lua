map = Map("mqtt_subscriber");

section = map:section(NamedSection, "mqtt_subscriber_sct", "mqtt_subscriber", "Settings")

flag = section:option(Flag, "enable", "Enable", "Enable program")

host = section:option(Value, "host", "Host", "Host address")
host.datatype = "ipaddr"

port = section:option(Value, "port", "Port", "Host port")
port.datatype = "port"

username = section:option(Value, "username", "Username")
username.maxlength = 20

password = section:option(Value, "password", "Password")
password.maxlength = 20
password.password = true
password.datatype = "credentials_validate"

--try

s2 = map:section(NamedSection, "mqtt_subscriber", "mqtt_subscriber", translate("Topic settings"), "")
s2:tab("topic", translate("topic"))

s2.template = "tsection"
s2.anonymous = true

function s2.cfgsections(self)
	return {"mqtt_subscriber"}
end

--topics

st = map:section(TypedSection, "topic", translate("Topics"), translate("") )
st.addremove = true
st.anonymous = true
st.template = "tblsection"
st.novaluetext = translate("There are no topics created yet.")

topic = st:option(Value, "topic", translate("Topic name"), translate(""))
topic.datatype = "string"
topic.maxlength = 65536
topic.placeholder = translate("Topic")
topic.rmempty = false
topic.parse = function(self, section, novld, ...)
	local value = self:formvalue(section)
	if value == nil or value == "" then
		self.map:error_msg(translate("Topic name can not be empty"))
		self.map.save = false
	end
	Value.parse(self, section, novld, ...)
end

qos = st:option(ListValue, "qos", translate("QoS level"), translate("The publish/subscribe QoS level used for this topic"))
qos:value("0", "At most once (0)")
qos:value("1", "At least once (1)")
qos:value("2", "Exactly once (2)")
qos.rmempty=false
qos.default="0"

return map