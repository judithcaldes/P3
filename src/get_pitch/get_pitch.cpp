/// @file

#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>

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
  std::map<std::string, docopt::value> args = docopt::docopt(
      USAGE,
      {argv + 1, argv + argc}, // array of arguments, without the program name
      true,                    // show help if requested
      "2.0");                  // version string

  std::string input_wav = args["<input-wav>"].asString();
  std::string output_txt = args["<output-txt>"].asString();

  // pero no modificamos docopt no? es solo añadir argumentso

  // Class template std::function is a general-purpose polymorphic function
  // wrapper. Instances of std::function can store, copy, and invoke any
  // CopyConstructible Callable target -- functions (via pointers thereto),
  // lambda expressions, bind expressions, or other function objects, as well as
  // pointers to member functions and pointers to data members.

  // std:: string es lo que en cualquier lugar diríamos string a secas: standard
  // string class provides support for such objects with an interface similar to
  // that of a standard container of bytes, but adding features specifically
  // designed to operate with strings of single-byte characters.

  float maxnorm = std::stof(args["--maxnorm"].asString());
  float minZCR = std::stof(args["--minZCR"].asString());
  float norm = std::stof(args["--norm"].asString());
  float maxpot = std::stof(args["--maxpot"].asString());
  float alfa = std::stof(args["--alfa"].asString());
  float umbral1 = std::stof(args["--umbral1"].asString());
  float umbral2 = std::stof(args["--umbral2"].asString());

  // Read input sound file

  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno)
         << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::RECT, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For
  /// instance, central-clipping or low pass filtering may be used.

  // Iterate for each frame and save values in f0 vector

  vector<float>::iterator iX;
  vector<float> f0;

  // float f=0.0;

  for (iX = x.begin(); iX < x.end(); iX++) {

    if (*iX<alpha && * iX> - alpha) {
      *iX = 0.0;
    }
    // printf("%f ",*iX);
  }

  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a
  /// median filter or time-warping may be used.

  float avgPitch = 0;
  int numMuestras = 0

      // FILTROS DIVERSOS que quitan cambios esporádicos de una sola muestra:

      vector<float>::iterator iF0; // creamos iterador de floats

  for (iF0 = f0.begin(); iF0 < f0.end();
       iF0++) { // hacemos for de principio a fin

    if (*iF0 != 0.0) {
      // a la que el iterador es distinto de 0
      avgPitch += *iF0; // le sumamos el valor obtenido
      numMuestras++; // y mantenemos cuenta del num de muestras para luego hacer
                     // la media

      if (iF0 == f0.begin()) {   // si estamos en el inicio
        if (*(iF0 + 1) == 0.0) { // y la siguiente muestra vale 0
          *iF0 = 0.0;            // la actual vale 0 tmb
        }
      } else if (iF0 == f0.end() - 1) { // si estamos en la penúltima muestra
        if (*(iF0 - 1) == 0.0) {        // y la anterior vale 0
          *iF0 = 0.0;                   // la penúltima (la actual) vale 0
        }
      } else {
        // si la anterior y la siguiente valen 0:
        if (*(iF0 - 1) == 0.0 && *(iF0 + 1) == 0.0) {
          // la acutal vale 0 también:
          *iF0 = 0.0;
        }
      }
    }
  }

  // filtro para ignorar los ceros esporádicos (que estén solos)

  for (iF0 = f0.begin(); iF0 < f0.end();
       iF0++) {                  // recorremos todas las muestras
    if (*iF0 == 0.0) {           // si algun valor es 0
      if (iF0 == f0.begin()) {   // y es la muestra inicial
        if (*(iF0 + 1) != 0.0) { // si la siguiente es distinca de 0
          *iF0 = *(iF0 + 1);     // el valor de la actual pasa a ser el de la
                                 // siguiente: ignoramos los 0
        }
      } else if (iF0 == f0.end() - 1) { // si estamos en la penúltima
        if (*(iF0 - 1) != 0.0) {        // si la anterior es distinta de 0
          *iF0 =
              *(iF0 - 1); // el valor de la actual pasa a ser el de la anterior
        }
      } else {
        if (*(iF0 - 1) != 0.0 &&
            *(iF0 + 1) !=
                0.0) { // si la anterior y la siguiente son distintos de 0
          *iF0 = (*(iF0 - 1) + *(iF0 + 1)) /
                 2; // el valor de la actual es la media entre ambos
        }
      }
    }
  }

  // ahora quitamos 0 de dos en dos
  for (iF0 = f0.begin(); iF0 < f0.end() - 3; iF0++) {
    // si la actual es distinta de 0 y las dos siguentes valen 0, y la 3a vuelve
    // a ser distinta de 0:
    if (*iF0 != 0.0 && *(iF0 + 1) == 0.0 && *(iF0 + 2) == 0.0 &&
        *(iF0 + 3) != 0.0) {
      // las dos intermedias que valían 0 pasan a valer la media enre la actual
      // y la 3a
      *(iF0 + 1) = (*(iF0) + *(iF0 + 3)) / 2;
      *(iF0 + 2) = *(iF0 + 1);
    }
  }

  avgPitch = avgPitch / numMuestras;

  for (iF0 = f0.begin(); iF0 < f0.end(); iF0++) {
    if (*iF0 >= avgPitch * th1) {
      *iF0 = avgPitch * th2;
    }
  }

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
