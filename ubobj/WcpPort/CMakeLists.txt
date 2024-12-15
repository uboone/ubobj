cet_make_library(
  SOURCE
  NuSelectionBDT.cxx
  NuSelectionContainment.cxx
  NuSelectionMatch.cxx
  NuSelectionTruth.cxx
  NuSelectionCharge.cxx
  NuSelectionKINE.cxx
  NuSelectionSTM.cxx
)

art_dictionary(
  DICTIONARY_LIBRARIES ubobj::WcpPort
)

install_headers()
install_fhicl()
install_source()
