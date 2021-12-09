# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( $ENV{NUTOOLS_INC} )


cet_make( LIBRARIES lardataobj_AnalysisBase

	  	    ROOT::EG
		    NO_DICTIONARY
        )

art_dictionary( DICTIONARY_LIBRARIES ubobj_CRT )

install_headers()
install_fhicl()
install_source()
