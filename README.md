## Proyecto de Parking IoT

### Descripción
Este proyecto consiste en una aplicación web para gestionar el estado de un estacionamiento (Parking) utilizando tecnología IoT. La aplicación permite visualizar las plazas de estacionamiento disponibles, ocupadas y reservadas en tiempo real, a través de una interfaz web interactiva.

### Archivos del Proyecto
1. `Proyecto_Final_PSS_IAC_lv.ino`
2. `Web_Parking_IoT.html`

### Contenido del archivo `Web_Parking_IoT.html`
El archivo `Web_Parking_IoT.html` contiene el código HTML, CSS y JavaScript necesarios para la visualización y actualización del estado del estacionamiento. A continuación se detallan las secciones principales del archivo:

#### Estructura HTML
- **Encabezado (`<header>`)**: Contiene el título de la página.
- **Contenedor Principal (`<div class="container">`)**: Incluye:
  - **Estado del Parking (`<div class="status">`)**: Muestra el número de plazas disponibles y ocupadas.
  - **Contenedor de Plazas (`<div class="plaza-container">`)**: Contiene las plazas de estacionamiento representadas como elementos `div`.

#### Estilos CSS
- Define el diseño visual de la página, incluyendo la fuente, colores de fondo, y estilos para los distintos estados de las plazas (disponible, ocupada, reservada).

#### Funcionalidad JavaScript
- **Fetch API**: Utiliza la API de ThingSpeak para obtener datos en tiempo real sobre las plazas ocupadas y reservadas.
- **Actualización Dinámica**: Basado en los datos obtenidos, actualiza el contenido de la página para reflejar el estado actual de las plazas de estacionamiento.
- **Interactividad**: Cambia el estilo de las plazas al pasar el ratón sobre ellas y muestra el estado mediante colores y símbolos.

### Instrucciones de Uso
1. **Preparación del Entorno**:
   - Asegúrate de tener acceso a Internet para que la página pueda realizar las solicitudes a la API de ThingSpeak.
   - Coloca la imagen `parking_background.jpg` en el mismo directorio que el archivo HTML para el fondo de la página.

2. **Ejecución**:
   - Abre el archivo `Web_Parking_IoT.html` en un navegador web.
   - La página mostrará el estado actual del estacionamiento, actualizando automáticamente las plazas disponibles y ocupadas.

3. **Configuración de la API**:
   - Verifica que las URLs de las solicitudes Fetch a ThingSpeak contienen la clave API correcta y el número de canal adecuado.
   - Ajusta el total de plazas de estacionamiento si es diferente a 3 (el valor utilizado en este ejemplo).

### Ejemplo de Funcionamiento
Al cargar la página, se realizarán dos solicitudes HTTP GET a ThingSpeak para obtener los datos más recientes de las plazas ocupadas y reservadas. La información se actualizará en la interfaz gráfica, mostrando las plazas disponibles en verde, ocupadas en rojo y reservadas con un icono de advertencia.

### Créditos
- Diseño de la interfaz y estilo: Desarrollado con CSS.
- Funcionalidad de actualización en tiempo real: Implementado con JavaScript y la API de ThingSpeak.
- Proyecto basado en tecnologías web y IoT para una solución de gestión de estacionamiento eficiente.

Para cualquier duda o sugerencia, por favor contacta al desarrollador del proyecto.
