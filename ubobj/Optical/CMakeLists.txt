cet_make_library(
  SOURCE
  Flash.cc
  FlashList.cc
  SubEvent.cc
  SubEventList.cc
)

art_dictionary(
  DICTIONARY_LIBRARIES ubobj::Optical
)

install_headers()
install_fhicl()
install_source()
