cet_make_library(
  SOURCE
  DAQHeaderTimeUBooNE.cxx
  SparseRawDigit.cxx
  LIBRARIES
  PUBLIC
  lardataobj::RawData
)

art_dictionary(
  DICTIONARY_LIBRARIES ubobj::RawData
)

install_headers()
install_fhicl()
install_source()
