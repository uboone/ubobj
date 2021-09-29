# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( $ENV{NUTOOLS_INC} )

cet_make(LIBRARIES
           ROOT::EG
           cetlib_except::cetlib_except
           NO_DICTIONARY
        )

art_dictionary( DICTIONARY_LIBRARIES ubobj_MuCS )

install_headers()
install_fhicl()
install_source()
