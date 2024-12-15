cet_make_library(
  SOURCE
  FlashMatch.cxx
  MCGhost.cxx
  SelectionResult.cxx
  TPCObject.cxx
  UBXSecEvent.cxx
  LIBRARIES
  PUBLIC
  lardataobj::RecoBase
)

art_dictionary(
  DICTIONARY_LIBRARIES
  ubobj::UBXSec
  nusimdata::SimulationBase
)

install_headers()
install_fhicl()
install_source()
