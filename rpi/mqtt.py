import serial
import paho.mqtt.client as mqtt

pico_serial = serial.Serial("/dev/ttyACM0", 115200)


def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {str(rc)}")


def on_message(client, userdata, msg):
    print("Received pump request")
    pico_serial.write('p'.encode())


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set("team07", "tjells123")
client.connect("10.0.0.10", 1883, 60)

client.subscribe("pico/button")

while True:
    data = str(pico_serial.readline().decode("utf-8").rstrip())

    plant_water_alarm, pump_water_alarm, moisture, light = data.split(
        ",")

    print(f"{plant_water_alarm} - {pump_water_alarm} - {moisture} - {light}")

    client.publish("pico/data/plantwateralarm", plant_water_alarm)
    client.publish("pico/data/pumpwateralarm", pump_water_alarm)
    client.publish("pico/data/moisture", moisture)
    client.publish("pico/data/light", light)

    client.loop()
