cet_make_library(
  SOURCE
  CRTHit.cc
  CRTTrack.cc
)

art_dictionary(
  DICTIONARY_LIBRARIES
  lardataobj::RecoBase
  nusimdata::SimulationBase
)

install_headers()
install_fhicl()
install_source()
