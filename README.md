PAV - P5: síntesis musical polifónica
=====================================

Obtenga su copia del repositorio de la práctica accediendo a [Práctica 5](https://github.com/albino-pav/P5) 
y pulsando sobre el botón `Fork` situado en la esquina superior derecha. A continuación, siga las
instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para crear una rama con el apellido de
los integrantes del grupo de prácticas, dar de alta al resto de integrantes como colaboradores del proyecto
y crear la copias locales del repositorio.

Como entrega deberá realizar un *pull request* con el contenido de su copia del repositorio. Recuerde que
los ficheros entregados deberán estar en condiciones de ser ejecutados con sólo ejecutar:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.sh
  make release
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A modo de memoria de la práctica, complete, en este mismo documento y usando el formato *markdown*, los
ejercicios indicados.

Ejercicios.
-----------

### Envolvente ADSR.

Tomando como modelo un instrumento sencillo (puede usar el InstrumentDumb), genere cuatro instrumentos que
permitan visualizar el funcionamiento de la curva ADSR.

* Un instrumento con una envolvente ADSR genérica, para el que se aprecie con claridad cada uno de sus
  parámetros: ataque (A), caída (D), mantenimiento (S) y liberación (R).

  En este caso se observa una envolvente ADSR genérica. Al principio aparece el ataque que es el trozo creciente de la gráfica, seguidamente aparece la caída que sucede después del ataque. Mientras el músico no libera la nota que está tocando esta se mantiene a una cierta intensidad, este tramo se llama de mantenimiento. Cúando el músico libera la nota aparece el tramo de liberación donde la onda se extinge.

  ![Alt text](img/Screenshot%20from%202023-01-24%2021-36-21.png?raw=true "Title")



* Un instrumento *percusivo*, como una guitarra o un piano, en el que el sonido tenga un ataque rápido, no
  haya mantenimiemto y el sonido se apague lentamente.
  - Para un instrumento de este tipo, tenemos dos situaciones posibles:
    * El intérprete mantiene la nota *pulsada* hasta su completa extinción.

      

      En este caso se observa como el músico mantiene la nota pulsada hasta la desaparición del sonido, la nota después del ataque desciende sin mantenimiento. Se llega a la extinción natural del sonido. 

      ![Alt text](img/2.png?raw=true "Title")

    * El intérprete da por finalizada la nota antes de su completa extinción, iniciándose una disminución
	  abrupta del sonido hasta su finalización.

      ![Alt text](img/3.png?raw=true "Title")
  - Debera representar en esta memoria **ambos** posibles finales de la nota.
* Un instrumento *plano*, como los de cuerdas frotadas (violines y semejantes) o algunos de viento. En
  ellos, el ataque es relativamente rápido hasta alcanzar el nivel de mantenimiento (sin sobrecarga), y la
  liberación también es bastante rápida.
  
  Por otro lado, los instrumentos de viento no tienen el tramo de caída, simplemente se desvanece el sonido cuando se termina la nota.
  ![Alt text](img/4.png?raw=true "Title")
Para los cuatro casos, deberá incluir una gráfica en la que se visualice claramente la curva ADSR. Deberá
añadir la información necesaria para su correcta interpretación, aunque esa información puede reducirse a
colocar etiquetas y títulos adecuados en la propia gráfica (se valorará positivamente esta alternativa).

### Instrumentos Dumb y Seno.

Implemente el instrumento `Seno` tomando como modelo el `InstrumentDumb`. La señal **deberá** formarse
mediante búsqueda de los valores en una tabla.

- Incluya, a continuación, el código del fichero `seno.cpp` con los métodos de la clase Seno.

  ```c
    #include <iostream>
    #include <math.h>
    #include "instrument_sin.h"
    #include "keyvalue.h"

    #include <stdlib.h>

    using namespace upc;
    using namespace std;

    InstrumentSin::InstrumentSin(const std::string &param)
        : adsr(SamplingRate, param)
    {
      bActive = false;
      x.resize(BSIZE);

      /*
        You can use the class keyvalue to parse "param" and configure your instrument.
        Take a Look at keyvalue.h
      */
      KeyValue kv(param);
      int N;
      FILE *f = fopen("tblfile.log", "a");

      if (!kv.to_int("N", N))
        N = 40;

      tbl.resize(N);
      float phase = 0, step = 2 * M_PI / (float)N;
      index = 0;
      for (int i = 0; i < N; ++i)
      {
        tbl[i] = sin(phase);
        ;
        fprintf(f, "%f\n", tbl[i]);
        phase += step;
      }
      fclose(f);
    }

    void InstrumentSin::command(long cmd, long note, long vel)
    {

      f0 = 440 * pow(2, (note - 69.) / 12);
      if (cmd == 9)
      {
        bActive = true;
        adsr.start();
        index = 0;
        phas = 0;
        increment = ((f0 / SamplingRate) * tbl.size());
        A = vel / 127.;
        phas = 0;
      }
      else if (cmd == 8)
      {
        adsr.stop();
      }
      else if (cmd == 0)
      {
        adsr.end();
      }
    }

    const vector<float> &InstrumentSin::synthesize()
    {
      if (not adsr.active())
      {
        x.assign(x.size(), 0);
        bActive = false;
        return x;
      }
      else if (not bActive)
        return x;
      FILE *fp;
      fp = fopen("xvector.log", "a");
      for (unsigned int i = 0; i < x.size(); ++i)
      {
        phas = phas + increment;

        // x[i] = A * tbl[round(phas)];

        // Amb interpolació
        x[i] = A * (tbl[floor(phas)] + (phas - floor(phas)) * (tbl[floor(phas + 1)] - tbl[floor(phas)]) / (floor(phas + 1) - floor(phas)));

        fprintf(fp, "%f\n", x[i]);
        while (phas >= tbl.size())
          phas = phas - tbl.size();
      }
      adsr(x); // apply envelope to x and update internal status of ADSR
      fclose(fp);
      return x;
      }

  ```
- Explique qué método se ha seguido para asignar un valor a la señal a partir de los contenidos en la tabla,
  e incluya una gráfica en la que se vean claramente (use pelotitas en lugar de líneas) los valores de la
  tabla y los de la señal generada.

  La tabla simplemente almacena en un vector de N posiciones sin(2*pi/N) y cada posición que se avanza corresponde a variar el argumento. float phas = 0, step = 2 * M_PI /(float)N; fase += paso;

  Para crear el instrumento Seno es necesario que la fase del seno comience por 0. Para empezar hay que introducir la f0 a la que se trabaja, utilizamos la siguiente fórmula: Nota=69+12·log2(f0/440 ) y aislamos f0. El valor se establece en la variable de incremento, este valor nos ayudará a iterar el argumento del seno cuando se usa la síntesis.


  ![Alt text](img/seno.png?raw=true "Title")

  Este seno hace referencia a la nota 'do' del archivo 'doremi'. Si mirásemos el re, mi etc... veríamos que la frecuencia asciende.

- Si ha implementado la síntesis por tabla almacenada en fichero externo, incluya a continuación el código
  del método `command()`.

### Efectos sonoros.

- Incluya dos gráficas en las que se vean, claramente, el efecto del trémolo y el vibrato sobre una señal
  sinusoidal. Deberá explicar detalladamente cómo se manifiestan los parámetros del efecto (frecuencia e
  índice de modulación) en la señal generada (se valorará que la explicación esté contenida en las propias
  gráficas, sin necesidad de *literatura*).


  Tremolo:
  
    Observamos que al primero no se le aplica (tal y como lo hemos decidido en el .sco. A las dos últimas notas tampoco).

    ![Alt text](img/treall.png?raw=true "Title")

    Si observamos detalladamente (con frecuencia 10Hz y A=0.5):

    ![Alt text](img/trezoom.png?raw=true "Title")

  Vibrato:

    ![Alt text](img/vial.png?raw=true "Title")
    
    Igual que antes, no se aplica ni a la primera ni dos últimas. En este casoe el identificador de efecto es 4. Con I=24 y f=2000 tenemos esto:

    ![Alt text](img/viz.png?raw=true "Title")

- Si ha generado algún efecto por su cuenta, explique en qué consiste, cómo lo ha implementado y qué
  resultado ha producido. Incluya, en el directorio `work/ejemplos`, los ficheros necesarios para apreciar
  el efecto, e indique, a continuación, la orden necesaria para generar los ficheros de audio usando el
  programa `synth`.

### Síntesis FM.

Construya un instrumento de síntesis FM, según las explicaciones contenidas en el enunciado y el artículo
de [John M. Chowning](https://web.eecs.umich.edu/~fessler/course/100/misc/chowning-73-tso.pdf). El
instrumento usará como parámetros **básicos** los números `N1` y `N2`, y el índice de modulación `I`, que
deberá venir expresado en semitonos.

- Use el instrumento para generar un vibrato de *parámetros razonables* e incluya una gráfica en la que se
  vea, claramente, la correspondencia entre los valores `N1`, `N2` e `I` con la señal obtenida.


  Con : 1	InstrumentFM	ADSR_A=0.1; ADSR_D=0.2; ADSR_S=0.3; ADSR_R=0.1; N=27; N2=4; I=4;

  ![Alt text](img/ss.png?raw=true "Title")

- Use el instrumento para generar un sonido tipo clarinete y otro tipo campana. Tome los parámetros del
  sonido (N1, N2 e I) y de la envolvente ADSR del citado artículo. Con estos sonidos, genere sendas escalas
  diatónicas (fichero `doremi.sco`) y ponga el resultado en los ficheros `work/doremi/clarinete.wav` y
  `work/doremi/campana.work`.

  clarinete

      1   InstrumentFM    ADSR_A=0.02; ADSR_D=0.1; ADSR_S=0.4; ADSR_R=0.1; N1=3; N2=2; I=4;

  campana 

      1  InstrumentFM    ADSR_A=0.01; ADSR_D=0.5; ADSR_S=0; ADSR_R=0; N1=5; N2=8; I=10;

  
  * También puede colgar en el directorio work/doremi otras escalas usando sonidos *interesantes*. Por
    ejemplo, violines, pianos, percusiones, espadas láser de la
	[Guerra de las Galaxias](https://www.starwars.com/), etc.

### Orquestación usando el programa synth.

Use el programa `synth` para generar canciones a partir de su partitura MIDI. Como mínimo, deberá incluir la
*orquestación* de la canción *You've got a friend in me* (fichero `ToyStory_A_Friend_in_me.sco`) del genial
[Randy Newman](https://open.spotify.com/artist/3HQyFCFFfJO3KKBlUfZsyW/about).

- En este triste arreglo, la pista 1 corresponde al instrumento solista (puede ser un piano, flautas,
  violines, etc.), y la 2 al bajo (bajo eléctrico, contrabajo, tuba, etc.).
- Coloque el resultado, junto con los ficheros necesarios para generarlo, en el directorio `work/music`.
- Indique, a continuación, la orden necesaria para generar la señal (suponiendo que todos los archivos
  necesarios están en directorio indicado).

      synth work/music/FriendInMe.orc work/music/ToyStory_A_Friend_in_me.sco work/music/Friend.wav

  
  FriendInMe.orc:


      #Piano
      1   InstrumentFM    ADSR_A=0.001; ADSR_D=0.1; ADSR_S=0.35; ADSR_R=0.25; I=20; N1 = 1; N2= 2;
      #Bajo 
      2   InstrumentFM    ADSR_A=0.02; ADSR_D=0.48; ADSR_S=0.6; ADSR_R=0.1; I=1.5; N1=2; N2=0.4;

También puede orquestar otros temas más complejos, como la banda sonora de *Hawaii5-0* o el villacinco de
John Lennon *Happy Xmas (War Is Over)* (fichero `The_Christmas_Song_Lennon.sco`), o cualquier otra canción
de su agrado o composición. Se valorará la riqueza instrumental, su modelado y el resultado final.
- Coloque los ficheros generados, junto a sus ficheros `score`, `instruments` y `efffects`, en el directorio
  `work/music`.
- Indique, a continuación, la orden necesaria para generar cada una de las señales usando los distintos
  ficheros.

      synth work/music/Hawai.orc work/music/Hawaii5-0.sco work/music/Hawai.wav
