cet_make_library(
  SOURCE
  MuCSDTOffset.cpp
  MuCSData.cpp
  MuCSRecoData.cpp
  LIBRARIES
  PUBLIC
  ROOT::MathCore
  cetlib_except::cetlib_except
)

art_dictionary(
  DICTIONARY_LIBRARIES
  ubobj::MuCS
)

install_headers()
install_fhicl()
install_source()
