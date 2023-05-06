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
		
Primeramente, usando el código desarrollado en la práctica 1, hemos representado la potencia y la tasa de cruces por cero (ZCR), el resultando es el siguiente, donde la ZCR es la gráfica superior, seguida de la potencia, y finalmente debajo del todo se encuentra el audio, en este caso prueba.wav:
		
<img width="1440" alt="image" src="https://user-images.githubusercontent.com/125259984/235954550-797e4888-c2b1-420e-a2d0-86f5d9da53be.png">

Vemos, como ya se ha explicado en prácticas previas, que como más sonoro es un segmento más potencia tiene. Además, hemos optado por añadir también la tasa de curces por cero porque ésta es otro buen criterio decisor de si una trama es sorda o sonora: los segmentos sordos, al tener menos potencia y ser más aleatorios, tienen una mayor ZCR dada por un constante "zigzag" de potencia alrededor del 0. En la gráfica también se puede observar el comportamiento explicado, en que en los puntos donde menos potencia se observa en la gráfica de en medio, son los que mayor ZCR muestran en la gráfica superior (como se puede apreciar, por ejemplo, en el instante temporal 0.7).
	
Configurando los paneles con el tamaño de ventana adecuado, obtenemos las siguientes gráficas:
		
**Segmento Sonoro:**	
		 	
- Una única ventana (15 ms):
		
<img width="1431" alt="image" src="https://user-images.githubusercontent.com/125259984/235961757-02568182-90e7-49ba-b80e-2796c0912168.png">

- Unas pocas ventanas (menos de 10):
		
<img width="1433" alt="image" src="https://user-images.githubusercontent.com/125259984/235962099-000beebb-6c3f-4a0d-a7cf-8e4e3f4b45fb.png">


**Segmento Sordo:**
		
- Una única ventana (15 ms):
		
<img width="1429" alt="image" src="https://user-images.githubusercontent.com/125259984/235962507-e40b3924-5920-4cfe-85b4-be0e196daf1a.png">


- Unas pocas ventanas (menos de 10):
		
<img width="1435" alt="image" src="https://user-images.githubusercontent.com/125259984/235962420-37d9284a-a29a-416b-9039-f1869a0ad4f8.png">

Como se puede observar, hemos obtenido unos -20 dB de potencia y un ZCR de alrededor de 0.5 en el caso de un sonido sonoro. En el caso sordo, sin embargo, la potencia obtenida ronda los -40 dB, y el ZCR es de 8. Es por ello que un buen umbral de potencia sería -30 dB, y de ZCR  4.25 (obtenidos en cada caso haciendo la media entre ambos valores). Para el conjunto de señales de training, sin embargo, hemos cogido un valor distino como umbral de potencia, pues nos daba mejores resultados (el umbral calculado ahora es el óptimo únicamente para la señal analizada, en los tramos concretos escogidos, y usando el algoritmo de cálculo de potencia de la práctica 1). Hemos elegido un umbral de -60 dB de potencia, como se puede observar en la figura de más adelante.

A continuación, y volviendo al código de la práctica 3, obtenemos los datos de rmaxnorm y r1norm poniendo a 1 el siguiente if:

 ``` bash
	 #if 1
    	if (r[0] > 0.0F)
  	cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << endl;
	#endif
	
 ```
	
Entonces, obtenemos un fichero de datos con rmaxnorm y r1norm mediante los comandos:

 ``` bash
	get_pitch prueba.wav prueba.f0 | cut -f 1 > definitivo_potencia_p3.pot 
 ```
 
  ``` bash
	get_pitch prueba.wav prueba.f0 | cut -f 2 > definitivo_r1_entre_r0.pot
 ```
 
  ``` bash
	 get_pitch prueba.wav prueba.f0 | cut -f 3 > definitivo_rlag_entre_r0.pot
 ```

A partir de los cuales obtenemos ficheros .pot de una sola columna, la que nos interese en cada caso. Podemos observar su apariencia haciendo un cat:

<img width="572" alt="image" src="https://user-images.githubusercontent.com/125259984/235990446-a658dc57-7179-43ba-82cb-809542252611.png">


Representando, finalmente, el resultado en WaveSurfer (con rmaxnorm arriba, y r1norm en medio), obtenemos la siguiente gráfica:
	
<img width="1440" alt="image" src="https://user-images.githubusercontent.com/125259984/236022917-06064580-4d62-4f9b-b96a-51cc2525e2d4.png">

Configurando los panenes con el tamaño de ventana adecuado, obtenemos las siguientes gráficas:
		
**Segmento Sonoro:**
		 	
- Una única ventana (15 ms):
		
<img width="1437" alt="image" src="https://user-images.githubusercontent.com/125259984/236023418-794adb59-de07-4315-8c8f-49073d73645f.png">

- Unas pocas ventanas (menos de 10):
		
<img width="1439" alt="image" src="https://user-images.githubusercontent.com/125259984/236023770-0f3b845f-e990-4cd3-b957-01f2c29b4538.png">

**Segmento Sordo:**
		
- Una única ventana (15 ms):	

<img width="1439" alt="image" src="https://user-images.githubusercontent.com/125259984/236023562-dc5f89f8-d121-425b-8876-8619ced2f091.png">

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/125259984/236024082-a77781b3-8081-49ba-a12e-bcb5a285682d.png">
		
- Unas pocas ventanas (menos de 10):

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/125259984/236023861-7223be18-d051-489c-a98a-a66ccea5d2b2.png">

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/125259984/236023986-9eaec65a-e917-4809-9e2a-1fcacb01cc23.png">

Además, para comprobar que efectivamente la potencia obtenida con el código de la práctica 1 y el de la 3 coinciden, se han representado juntas:

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/125259984/236021728-726db2d5-166a-4a3d-9a4c-1c06fb7d85d4.png">

Vemos como, aunque estén escalados de manera distinta en cantidad de decibelios y la de la práctica 3 tenga un corte a -100 dB, ambas gráficas muestran la misma envolvente, con la misma cantidad de picos.

Como se puede observar, hemos obtenido en el caso de un sonido sonoro rmaxnorm de aproximadamente 0.85, y un r1norm de prácticamente 1. En el caso sordo, sin embargo, observamos un rmaxnorm de alrededor de 0.15, y un r1norm de aproximadamente 0.75. Es por ello que un buen umbral de rmaxnorm sería 0.5, y de r1norm 0.875 (obtenidos en cada caso haciendo la media entre ambos valores). Nuevamente, para el conjunto de señales de training hemos cogido unos valores distinos, pues nos daban mejores resultados. Hemos elegido los siguientes parámetros:

<img width="892" alt="image" src="https://user-images.githubusercontent.com/125259984/236640651-87650f2d-b9ac-4f0b-ae83-6eb73274af92.png">

Por último, cabe destacar que para representar de manera correcta los valores obtenidos del código de la práctica 3 (potencia, rmaxnorm, r1norm) se ha tenido que hacer un cambio de escala para ajustarlo a la ventana usada de 15 ms. Para ello hemos tenido que cambiar en las propiedades del Data Plot el valor de *Data Spacing* de la siguiente manera:

<img width="642" alt="image" src="https://user-images.githubusercontent.com/125259984/236021497-54a2ab77-70f6-42ff-9c41-717b0f72cc91.png">

- Use el estimador de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica ilustrativa del resultado de ambos estimadores.

Primeramente, para obtener el pitch mediante nuestro código, hemos seguido un proceso muy similar al realizado para los parámetros anteriores, pero cambiando el if para que incorporase esta salida extra:

```bash
#if 1
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << '\t' << (float) samplingFreq/(float) lag << '\t' << endl;
#endif
```
Entonces, hemos obtenido el fichero .pot resultante con el siguiente filtro:

```bash
get_pitch prueba.wav prueba.f0 | cut -f 4 > definitivo_pitch.pot
```
Podemos ver el resultado mediante un cat:

<img width="639" alt="image" src="https://user-images.githubusercontent.com/125259984/236641540-cf603197-8412-4944-aeb5-e5f198f5cc1a.png">

Para calcular el pitch mediante WaveSurfer, por otra parte, simplemente hemos creado un panel y aplicado la siguiente configuración:

<img width="310" alt="image" src="https://user-images.githubusercontent.com/125259984/236641577-56eb9abd-0fdc-4c02-8f95-6b2d9f74bb8b.png">

Obtendiendo, finalmente, el siguiente resultado:

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/125259984/236641586-ad488dc3-328a-415d-8946-0ccbbb4d71ed.png">

Observamos como, efectivamente, los pitch de ambas gráficas parecen coincidir. La diferencia más notoria, sin embargo, es la continuidad. Como no hemos aplicado ningun tipo de filtrado, nuestro programa calcula el pitch para absolutamente cada instante de la señal. 
     
Aunque puede usar el propio Wavesurfer para obtener la representación, se valorará el uso de alternativas de mayor calidad (particularmente Python).
		
Hemos calculado el ptich también en Python, pues efectivamente los resultados de wavesurfer no nos daban tanta información como los de python, que nos han permitido representar un espectrograma mucho más preciso. El código creado ha sido el siguiente:

```bash
import librosa
import librosa.display
import matplotlib.pyplot as plt

filename = 'prueba.wav'

# Cargar el archivo de audio
y, sr = librosa.load(filename)

# Calcular el pitch
pitch, _ = librosa.core.piptrack(y=y, sr=sr)

# Graficar el pitch
plt.figure(figsize=(15, 5))
librosa.display.specshow(pitch, sr=sr, y_axis='linear', x_axis='time')
plt.colorbar()
plt.title('Pitch')
plt.tight_layout()
plt.show()
```

De donde hemos obtenido la siguiente gráfica de pitch para el audio prueba.wav:

<img width="1317" alt="image" src="https://user-images.githubusercontent.com/125259984/236641884-e2493f82-763f-4b96-9008-544d0f6c74b2.png">
 
Para desarrollar el código hemos usado google colab, y el audio lo hemos importado de la siguiente manera:

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/125259984/236642156-ac04427b-8e7e-4549-893c-1ab6099271ae.png">

  * Optimice los parámetros de su sistema de estimación de pitch e inserte una tabla con las tasas de error y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..
	
Los parámetros usados ya se han dado previamente, y el comando necesario para calcular lo que se nos pide es el siguiente:

```bash
pitch_evaluate pitch_db/train/*.f0ref | grep TOTAL
```
Hemos obtenido el siguiente valor para la evaluación de la base de datos:

<img width="899" alt="image" src="https://user-images.githubusercontent.com/125259984/236642412-a4920598-fe05-4007-93e4-94f2fbc7ae9d.png">

Para nuestro audio de prueba obtenemos resultados ligeramente mejores:

<img width="899" alt="image" src="https://user-images.githubusercontent.com/125259984/236642463-55b6ebdd-43b1-40b6-992a-e8fbb529f6d0.png">



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
