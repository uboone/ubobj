cet_make_library(
  SOURCE
  EventMixingSummary.cxx
  LIBRARIES
  PUBLIC
  canvas::canvas
)

art_dictionary()

install_headers()
install_fhicl()
install_source()
