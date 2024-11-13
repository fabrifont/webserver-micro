import tkinter as tk
import requests
import json

def enviar_datos():
    temperatura = entry_temperatura.get()
    humedad = entry_humedad.get()
    color = entry_color.get()
    velocidad = entry_velocidad.get()

    datos = {
        "temperatura": temperatura,
        "humedad": humedad,
        "color": color,
        "velocidad": velocidad
    }
    
    json_datos = json.dumps(datos)

    requests.post('http://192.168.1.177/datos', json_datos)

root = tk.Tk()
root.title("Simular Sensor - Servidor Web")
root.minsize(650, 400)

label_titulo = tk.Label(root, text="Microprocesadores")
label_titulo.config(font=("Verdana", 20))
label_titulo.pack()

label_subtitulo = tk.Label(root, text="6to Electro IPS")
label_subtitulo.config(font=("Verdana", 20))
label_subtitulo.pack()

label_nombre1 = tk.Label(root, text="Fontanarrosa")
label_nombre1.config(font=("Verdana", 12))
label_nombre1.pack()

label_nombre2 = tk.Label(root, text="Dominguez")
label_nombre2.config(font=("Verdana", 12))
label_nombre2.pack()


label_color = tk.Label(root, text="Color:")
label_color.pack()
entry_color = tk.Entry(root)
entry_color.pack()

label_temperatura = tk.Label(root, text="Temperatura:")
label_temperatura.pack()
entry_temperatura = tk.Scale(root, from_=-50, to=50, orient="horizontal", tickinterval=0.1, length=200)
entry_temperatura.set(0)
entry_temperatura.pack()

label_humedad = tk.Label(root, text="Humedad:")
label_humedad.pack()
entry_humedad = tk.Scale(root, from_=0, to=100, orient="horizontal", tickinterval=0.1, length=200)
entry_humedad.pack()

label_velocidad = tk.Label(root, text="Velocidad:")
label_velocidad.pack()
entry_velocidad = tk.Scale(root, from_=0, to=150, orient="horizontal", tickinterval=0.1, length=200)
entry_velocidad.pack()

boton_enviar = tk.Button(root, text="Enviar", command=enviar_datos)
boton_enviar.pack()

root.mainloop()
