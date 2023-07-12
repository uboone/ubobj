cet_make_library(
  SOURCE
  ubdaqSoftwareTriggerData.cpp
)

art_dictionary(
  DICTIONARY_LIBRARIES ubobj::Trigger
)

install_headers()
install_fhicl()
install_source()
