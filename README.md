PAV - P3: estimación de pitch
=============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la estimación de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.

```bash

  unsigned int l=0;
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
    //cálculo de la autocorrelación

    for (unsigned int l = 0; l < r.size(); ++l) {
      /// \TODO Compute the autocorrelation r[l]
      /// \DONE Autocorrelación calculada
      /// - Inicializamos la autocorrelación a 0
      /// - Acumulamos los productos cruzados \f$\sum_{n=0}^{N-l} x[n]x[n+l]\f$
      /// - Dividimos por el numero de muestras

      for (unsigned int n = 0; n < x.size()-l; ++n) {
          r[l]+=x[n]*x[n+l];
      }
      r[l] = r[l]/x.size();
    }

    if (r[0] == 0.0F){ //to avoid log() and divide zero 
      r[0] = 1e-10; 
    }
  }
  ```
  
   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un segmento de
     unos 30 ms de un fonema sonoro y su periodo de pitch; y, en otro *subplot*, se vea con claridad la
	 autocorrelación de la señal y la posición del primer máximo secundario.

	 NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
	 hacerlo. Se valorará la utilización de la biblioteca matplotlib de Python.

Grabamos un tono sonoro de 30 ms con wavesurfer:

![image](https://user-images.githubusercontent.com/125259801/235349037-bcd040ae-12f2-4125-b6d9-78f0be219ca2.png)

Para crear las gráficas escribimos el siguiente código de MATLAB:

```bash
%Gráficas

[audio,Fs] = audioread('fonema_sonoro_30ms.wav');
t = seconds(0:1/Fs:(size(audio,1)-1)/Fs);

audio(:,2) = [];

subplot(2,1,1);
plot(t,audio)
title('30ms de un fonema sonoro')
xlabel("Time")
ylabel("Amplitude")
%xlim("tight")
%ylim([-1 1])


r = xcorr(audio,audio);
a=length(r)+1;
r_0=zeros(a/2,1);

for i=1:a/2
    r_0(i,1) = r(a/2+i-1,1);
end

subplot(2,1,2); 
plot(r_0)
title('Autocorrelación ')
xlabel("Time")
ylabel("Amplitude")
```

Los resultados son los siguientes:

![image](https://user-images.githubusercontent.com/125259801/235349024-1f0e72d7-95e4-49e7-91ff-bba90a836b0a.png)

   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.
     
```bash
    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin(), iRMax = iR;

    /// \TODO 
	/// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	/// Choices to set the minimum value of the lag are:
	///    - The first negative value of the autocorrelation.
	///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
	/// In either case, the lag should not exceed that of the minimum value of the pitch.
   /// \DONE 
	///Localizando el primer máximo secundario de la autocorrelación

  for (iR = iRMax = r.begin() + npitch_min; iR <= r.begin() + npitch_max; iR ++){
    if(*iR >*iRMax){
      iRMax = iR;
    }
  }

    unsigned int lag = iRMax - r.begin();
```

   * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.
   
```bash
bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.

    /// \DONE 
    /// Si se cumplen las 3 condiciones siguientes, se trata de un sonido sonoro:
    /// - La potencia es superior a nuestro umbral
    /// - El primer valor de la autocorrelación es superior a nuestro umbral
    /// - El valor máximo de la autocorrelación supera nuestro umbral máximo
    /// Si alguna de estas condiciones falla se trata de un sonido sordo
    
    if(pot > this->u_pot && r1norm > this->u_r1 && rmaxnorm > this->u_max){ 
      return false; //sonoro
    }else{
      return true; //sordo
    }
  }
  ```
  
   * Puede serle útil seguir las instrucciones contenidas en el documento adjunto `código.pdf`.

- Una vez completados los puntos anteriores, dispondrá de una primera versión del estimador de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la estimación de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

		Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
		en esta práctica es de 15 ms.

      - Use el estimador de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos estimadores.
     
		Aunque puede usar el propio Wavesurfer para obtener la representación, se valorará
	 	el uso de alternativas de mayor calidad (particularmente Python).
  
  * Optimice los parámetros de su sistema de estimación de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  estimador a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del estimador. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la estimación de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de estimación
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, diezmado, *center clipping*, etc.
  
Hemos utilizado la técnica de Center Clipping utilizando como referencia la siguiente web: http://notedetection.weebly.com/center-clipping.html

```bash
  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For
  /// instance, central-clipping or low pass filtering may be used.
  /// \DONE Hemos utilizado Center-Cliping

  // Iterate for each frame and save values in f0 vector

  vector<float>::iterator iX;
  vector<float> f0;

  float valor_max = *std::max_element(x.begin(), x.end()); //devuelve el valor de la amplitud máxima de la senyal de audio
  float alpha = 0.03 * valor_max; //fijamos un valor de alpha a partir del valor máximo del audio

  for (iX = x.begin(); iX < x.end(); iX++){ //Por cada trama iX se hace lo siguiente
    if(abs(*iX) < alpha){
      *iX = 0;
    }
    if(*iX > alpha){
      *iX = *iX - alpha;
    }
    if(*iX < -alpha){
      *iX = *iX + alpha;
    }
  }

  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }
  ```
  
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  
Hemos utilizado el filtro de mediana:
```bash
  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a
  /// median filter or time-warping may be used.
  /// \DONE Hemos utilizado el filtro de mediana

  // Definimos el tamaño de la ventana de filtrado
  const int MEDIAN_WINDOW = 4; //Utilizamos 6 pero habrán 7 números dentro de la ventana

  // Creamos un vector temporal para almacenar los valores filtrados
  vector<float> f0_filtered(f0);

  // Aplicamos el filtro de mediana a cada valor de f0
  for (long unsigned int i = MEDIAN_WINDOW/2; i < f0.size() - MEDIAN_WINDOW/2; i++) {
    // Creamos un vector temporal con los valores a filtrar
    vector<float> window;

    // Agregamos los valores a la ventana
    for (long unsigned int j = i - MEDIAN_WINDOW/2 ; j <= i + MEDIAN_WINDOW/2 ; j++) {
      
      // Agregamos el valor a la ventana
      window.push_back(f0[j]);
      
    }
    
    // Ordenamos la ventana
    std::sort(window.begin(), window.end());
    
    
    // Tomamos el valor central de la ventana como valor filtrado
    f0_filtered[i] = window[MEDIAN_WINDOW/2];
    
  }

  // Sobrescribimos el vector original con los valores filtrados
  f0 = f0_filtered;
  ```
  
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el estimador, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS%20Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al estimador. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
   

Evaluación *ciega* del estimador
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
estimador con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
