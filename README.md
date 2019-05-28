# Proyecto en QT utilizando OpenMP para una simulación de una pizzeria

## MTI - Programación paralela y multicore



 Pizzería a domicilio

El sistema consiste en la simulación de una pizzería a domicilio

 Cocineros:
	Cada cocinero se dedica a preparar pizzas, según van llegando pedidos.
	Cada vez que una pizza está hecha, el cocinero la coloca sobre una bandeja para su reparto.
	
	
	

 Repartidores:
	A su vez, cada repartidor espera a que haya una pizza sobre la bandeja, la retira y se la lleva al cliente correspondiente.
	Tras ello regresa a la pizzería y espera por una nueva pizza en la bandeja. Como añadido, la bandeja tiene una capacidad limitada: no puede haber más de N pizzas pendientes de reparto. 
	
Resolver:
	Lo que queremos resolver es que un repartidor se bloquee mientras no haya pizzas disponibles en la bandeja; y que un cocinero se bloquee si cuando quiere colocar una pizza en la bandeja se la encuentra llena.
	
	• 3 cocineros
	• 2 repartidores
	• El tiempo de elaboración de la pizza será un numero aleatorio de 1 - 4 seg
	• El tiempo de entrega del repartidor será de 5 - 9 segs para cada pizza entregada

Mostrar en pantalla, la lista de pizzas terminadas, en preparación y en espera de ser llevadas.

- Es un trabajo de equipo, máximo 4 personas
- Subir el archivo fuente en OpenMP adjuntando el nombre completo de cada integrante
- La interfaz gráfica debe ilustrar los procesos antes citados
- Debe ser resiliente


##### Para ver el demo, click en la imagen. =>
<a style="float:right" href="https://vakito.com/mti/assets/pizza.mp4" target="_blank">
  <img alt="PicSciP Demo Video" src="http://vakito.com/mti/assets/watson-python-tts-stt-assistant.JPG" width="728" height="456" />
</a>


Desde <https://classroom.google.com/c/Mjc0NjI2NTQ0OTRa/a/MzU3NDE2OTI1ODNa/details> 

### Todos

 - Documentar
 - Pruebas

License
----

MIT


**Software Libre! :)**
