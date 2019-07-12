from flask import Flask, abort, request, render_template, Response
import paho.mqtt.client as mqtt
import cv2 
import numpy as np
 
DEBUG=False

app = Flask(__name__)

dataX = []
dataY = []
dataZ = []

def draw():
  global dataX, dataY, dataZ 
  while True:
    myimg = np.zeros((768,1024,3), np.uint8)
    myimg = np.asarray(myimg)
    for point in range(len(dataX)-1):
      cv2.circle(myimg,(point,int(dataX[point]*100+100)), 1, (255,0,0), -1)
      cv2.circle(myimg,(point,int(dataY[point]*100+100)), 1, (0,255,0), -1)
      cv2.circle(myimg,(point,int(dataZ[point]*100+100)), 1, (0,0,255), -1)
    # disable for OpenCV imshow
    ret, jpeg = cv2.imencode('.jpg', myimg)
    try:
      yield(b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + jpeg.tobytes() + b'\r\n')
    except NameError as e:
      print(e)


def on_message(client, userdata, message):
    global dataX, dataY, dataZ 
    dataX.append(float(str(message.payload.decode("utf-8")).split(":")[2].split(",")[0].strip()))
    dataY.append(float(str(message.payload.decode("utf-8")).split(":")[3].split(",")[0].strip()))
    dataZ.append(float(str(message.payload.decode("utf-8")).split(":")[4].split(",")[0][:-1].strip()))
    if len(dataX) > 1030:
      del dataX[0]
      del dataY[0]
      del dataZ[0]
    #dataRecorded += str(message.payload.decode("utf-8"))
    #print("message topic=",message.topic)
    #print("message qos=",message.qos)
    #print("message retain flag=",message.retain)


################################################################################
@app.route('/')
def index():
  return render_template('index.html')


@app.route('/video')
def video():
  return Response(draw(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    client = mqtt.Client()
    broker_address="192.168.1.188" 
    espData = 'Accelerometer'

    client = mqtt.Client("P1")     #create new instance
    client.connect(broker_address) #connect to broker
    client.subscribe(espData)
    client.on_message=on_message   #attach function to callback
    print(client)
    client.loop_start()

    #write SVG
    #dwg = svgwrite.Drawing('/static/x.svg', size=("500px", "500px"))
    #dwg.add(dwg.text('Test', insert=(0, 0.2)))
    #dwg.save
    draw()

    app.run(host='0.0.0.0', port=80)
