cet_make_library(
  SOURCE
  BooNEInfo.cc
  LIBRARIES
  PUBLIC
)

art_dictionary(
  DICTIONARY_LIBRARIES
  lardataobj::RecoBase
  nusimdata::SimulationBase
)

install_headers()
install_source()
