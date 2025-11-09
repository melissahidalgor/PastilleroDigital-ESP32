document.addEventListener('DOMContentLoaded', () => {
    const formularioMedicamento = document.getElementById('formulario-medicamento');
    const tablaMedicamentosBody = document.getElementById('tabla-medicamentos');
    const listaProximasTomas = document.getElementById('lista-proximas-tomas');
    const listaPastillasTomadas = document.getElementById('lista-pastillas-tomadas');

    let medicamentos = cargarMedicamentosLocales(); 
    let pastillasTomadas = cargarPastillasTomadasLocales();
    let proximasTomasCalculadas = []; 

    function renderizarMedicamentos() {
        tablaMedicamentosBody.innerHTML = '';
        medicamentos.forEach((medicamento, index) => {
            const fila = tablaMedicamentosBody.insertRow();
            fila.insertCell().textContent = medicamento.nombre;
            fila.insertCell().textContent = medicamento.dosis;
            fila.insertCell().textContent = medicamento.frecuencia;
            fila.insertCell().textContent = medicamento.fecha_inicio;
            fila.insertCell().textContent = medicamento.duracion;
            fila.insertCell().textContent = medicamento.fecha_fin; 
            const celdaAcciones = fila.insertCell();
            const botonEliminar = document.createElement('button');
            botonEliminar.textContent = 'Eliminar';
            botonEliminar.addEventListener('click', () => eliminarMedicamento(index));
            celdaAcciones.appendChild(botonEliminar);
        });
    }

    formularioMedicamento.addEventListener('submit', (event) => {
        event.preventDefault();
        const nombre = document.getElementById('nombre').value;
        const dosis = document.getElementById('dosis').value;
        const frecuencia = parseInt(document.getElementById('frecuencia').value, 10);
        const fecha_inicio_str = document.getElementById('fecha_inicio').value;
        const duracion = parseInt(document.getElementById('duracion').value, 10);
        const primera_toma = document.getElementById('primera_toma').value;

        const fechaInicioObj = new Date(fecha_inicio_str);
        const fechaFinObj = new Date(fechaInicioObj);
        fechaFinObj.setDate(fechaFinObj.getDate() + duracion);
        const fecha_fin = fechaFinObj.toISOString().split('T')[0];

        medicamentos.push({
            nombre,
            dosis,
            frecuencia,
            fecha_inicio: fecha_inicio_str,
            duracion,
            fecha_fin,
            primera_toma,
        });

        guardarMedicamentosLocales();
        formularioMedicamento.reset();
        renderizarMedicamentos();
        actualizarProximasTomas();
    });

    const botonBorrarHistorial = document.getElementById('boton-borrar-historial');
    if (botonBorrarHistorial) {
        botonBorrarHistorial.addEventListener('click', () => {
            if (confirm('¿Estás seguro de que quieres borrar el historial de pastillas tomadas?')) {
                pastillasTomadas = []; // Vaciar el array en memoria
                localStorage.removeItem('pastillasTomadas'); // Eliminar del localStorage
                renderizarPastillasTomadas(); // Actualizar la visualización
                console.log('Historial de pastillas tomadas borrado.');
            }
        });
    } else {
        console.warn('No se encontró el botón con ID "boton-borrar-historial".');
    }

    function eliminarMedicamento(index) {
        if (confirm('¿Estás seguro de que quieres eliminar este medicamento?')) {
            medicamentos.splice(index, 1);
            guardarMedicamentosLocales();
            renderizarMedicamentos();
            actualizarProximasTomas();
        }
    }

    function renderizarPastillasTomadas() {
        listaPastillasTomadas.innerHTML = '';
        pastillasTomadas.forEach(toma => {
            const listItem = document.createElement('li');
            listItem.textContent = `Se tomó ${toma.nombre} el ${toma.fechaTomada} a las ${toma.horaTomada} (programado a las ${toma.horaProgramada})`;
            listaPastillasTomadas.appendChild(listItem);
        });
    }

    function verificarNuevaToma() {
        fetch('/get_toma_notificacion')
            .then(response => response.text())
            .then(data => {
                if (data === 'toma_realizada') {
                    console.log('Notificación de toma realizada recibida.');
    
                    if (proximasTomasCalculadas.length > 0) {
                        console.log('proximasTomasCalculadas.length > 0');
                        const proximaToma = proximasTomasCalculadas[0];
                        const ahora = new Date();
                        const horaTomada = ahora.toLocaleTimeString();
                        const fechaTomada = ahora.toLocaleDateString(); // Obtener la fecha
    
                        // Agregar al historial de pastillas tomadas
                        pastillasTomadas.push({
                            nombre: proximaToma.nombre,
                            horaTomada: horaTomada,
                            fechaTomada: fechaTomada, // Guardar la fecha
                            horaProgramada: proximaToma.fechaHora.toLocaleTimeString().slice(0, 5)
                        });
                        guardarPastillasTomadasLocales();
                        renderizarPastillasTomadas();
                
                        console.log('Tomada:', proximaToma.nombre);
                        proximasTomasCalculadas.shift(); // Eliminar la toma registrada

                        renderizarProximasTomas();

                        actualizarProximasTomas();

                    } else {
                        console.log('No había próxima toma para registrar.');
                    }
                }
            })
            .catch(error => {
                console.error('Error al verificar la notificación de toma:', error);
            });
    }


    function enviarProximaToma(nombre, hora, fecha) {
        console.log('Enviando:', nombre, hora, fecha);
        const url = `/recibir_proxima_toma?nombre=${encodeURIComponent(nombre)}&hora=${encodeURIComponent(hora)}&fecha=${encodeURIComponent(fecha)}`;
    
        fetch(url)
          .then(response => response.text())
          .then(data => {
            console.log('Respuesta del ESP32:', data);
          })
          .catch(error => {
            console.error('Error al enviar la próxima toma:', error);
          });
    }

    function renderizarProximasTomas() {
        listaProximasTomas.innerHTML = '';
        proximasTomasCalculadas.forEach(toma => {
            const listItem = document.createElement('li');
            listItem.textContent = toma.texto;
            listaProximasTomas.appendChild(listItem);
        });
    }

    function actualizarProximasTomas() {
        console.log('actualizarProximasTomas() ejecutándose a las:', new Date().toLocaleTimeString());
        listaProximasTomas.innerHTML = '';
        const ahora = new Date();
        const ahoraLocal = new Date(ahora.toLocaleString('en-US', { timeZone: Intl.DateTimeFormat().resolvedOptions().timeZone }));
        let nuevasProximasTomasCalculadas = [];
    
        medicamentos.forEach(medicamento => {
            const inicioFechaSolo = new Date(medicamento.fecha_inicio);
            const [inicioHora, inicioMinuto] = medicamento.primera_toma.split(':').map(Number);
            inicioFechaSolo.setHours(inicioHora);
            inicioFechaSolo.setMinutes(inicioMinuto);
            inicioFechaSolo.setSeconds(0);
            inicioFechaSolo.setMilliseconds(0);
            let fechaActualToma = new Date(inicioFechaSolo);
    
            const finFecha = new Date(medicamento.fecha_fin);
            finFecha.setHours(23);
            finFecha.setMinutes(59);
            finFecha.setSeconds(59);
            finFecha.setMilliseconds(999);
    
            while (fechaActualToma <= finFecha) {
                if (fechaActualToma > ahoraLocal) {
                    const potencialProximaToma = {
                        nombre: medicamento.nombre,
                        fechaHora: new Date(fechaActualToma),
                        texto: `Tomar ${medicamento.nombre} a las ${fechaActualToma.toLocaleTimeString()} el ${fechaActualToma.toLocaleDateString()}`
                    };
    
                    // Verificar si esta toma ya está registrada en pastillasTomadas
                    const yaTomada = pastillasTomadas.some(tomaRegistrada => {
                        return (
                            tomaRegistrada.nombre === potencialProximaToma.nombre &&
                            // Comparar las horas y minutos (puedes ajustar la tolerancia si es necesario)
                            tomaRegistrada.horaProgramada === potencialProximaToma.fechaHora.toLocaleTimeString().slice(0, 5) &&
                            tomaRegistrada.fechaTomada === potencialProximaToma.fechaHora.toLocaleDateString()
                        );
                    });
    
                    if (!yaTomada) {
                        nuevasProximasTomasCalculadas.push(potencialProximaToma);
                    }
                }
                fechaActualToma.setTime(fechaActualToma.getTime() + medicamento.frecuencia * 60 * 60 * 1000);
            }
        });
    
        nuevasProximasTomasCalculadas.sort((a, b) => a.fechaHora - b.fechaHora);
        proximasTomasCalculadas = nuevasProximasTomasCalculadas;
        renderizarProximasTomas();
    
        // Enviar la primera próxima toma al ESP32
        if (proximasTomasCalculadas.length > 0) {
            const primeraProximaToma = proximasTomasCalculadas[0];
            const [hora, minutos, segundos] = primeraProximaToma.fechaHora.toLocaleTimeString().split(':');
            const fecha = primeraProximaToma.fechaHora;
            const año = fecha.getFullYear();
            const mes = (fecha.getMonth() + 1).toString().padStart(2, '0');
            const dia = fecha.getDate().toString().padStart(2, '0');
    
            enviarProximaToma(
                primeraProximaToma.nombre,
                `${hora}:${minutos}`,
                `${año}-${mes}-${dia}`
            );
        } else {
            enviarProximaToma('No hay proximas tomas', '', '');
        }
    }
 
    // Funciones para guardar en localStorage
    function guardarMedicamentosLocales() {
        localStorage.setItem('medicamentos', JSON.stringify(medicamentos));
    }

    function guardarPastillasTomadasLocales() {
        localStorage.setItem('pastillasTomadas', JSON.stringify(pastillasTomadas));
    }

    // Funciones para cargar desde localStorage
    function cargarMedicamentosLocales() {
        const medicamentosGuardados = localStorage.getItem('medicamentos');
        return medicamentosGuardados ? JSON.parse(medicamentosGuardados) : [];
    }

    function cargarPastillasTomadasLocales() {
        const pastillasTomadasGuardadas = localStorage.getItem('pastillasTomadas');
        return pastillasTomadasGuardadas ? JSON.parse(pastillasTomadasGuardadas) : [];
    }

    // Inicializar la visualización
    renderizarMedicamentos();
    renderizarPastillasTomadas();
    actualizarProximasTomas();

    setInterval(actualizarProximasTomas, 60000);
    setInterval(verificarNuevaToma, 15000);
});