/// @file

#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <algorithm> // Incluimos la biblioteca para usar la función std::sort
#include <cmath>

#include "pitch_analyzer.h"
#include "wavfile_mono.h"

#include "docopt.h"

#define FRAME_LEN 0.030   /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -p, --u_pot=REAL    Umbral de potencia para la determinación de sonoro/sordo [default: -1.e6]
    -1, --u_r1=REAL     Umbral de la autocorrelación de 1 para la determinación de sonoro/sordo [default: 0.7]
    -m, --u_rmax=REAL   Umbral en el máximo de la autocorrelación para la determinación de sonoro/sordo [default: 0.4]

    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
  /// \TODO
  ///  Modify the program syntax and the call to **docopt()** in order to
  ///  add options and arguments to the program.
  /// \DONE Hemos añadido u_pot, u_r1 y u_rmax
  std::map<std::string, docopt::value> args = docopt::docopt(
      USAGE,
      {argv + 1, argv + argc}, // array of arguments, without the program name
      true,                    // show help if requested
      "2.0");                  // version string

        std::string input_wav = args["<input-wav>"].asString();

        std::string output_txt = args["<output-txt>"].asString();

  float u_r1 = std::stof(args["--u_r1"].asString());

  float u_rmax = std::stof(args["--u_rmax"].asString());
  float u_pot = std::stof(args["--u_pot"].asString());
 

  // Class template std::function is a general-purpose polymorphic function
  // wrapper. Instances of std::function can store, copy, and invoke any
  // CopyConstructible Callable target -- functions (via pointers thereto),
  // lambda expressions, bind expressions, or other function objects, as well as
  // pointers to member functions and pointers to data members.

  // std:: string es lo que en cualquier lugar diríamos string a secas: standard
  // string class provides support for such objects with an interface similar to
  // that of a standard container of bytes, but adding features specifically
  // designed to operate with strings of single-byte characters.

  // Read input sound file

  unsigned int rate; //frecuencia de muestreo
  vector<float> x; //señal de audio
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno)
         << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::RECT, 50, 500, u_pot, u_r1, u_rmax);

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

#define CUALQUIERCOSA 1
#ifdef CUALQUIERCOSA

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

  #endif

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " ("
         << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; // pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX)
    os << *iX << '\n';
  os << 0 << '\n'; // pitch at t=Dur

  return 0;
}
