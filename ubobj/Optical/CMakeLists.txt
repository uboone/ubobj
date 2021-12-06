# for classes_def.xml!!

cet_make( LIBRARIES
	  	    ROOT::EG
		    NO_DICTIONARY
)

art_dictionary( DICTIONARY_LIBRARIES ubobj_Optical )

install_headers()
install_fhicl()
install_source()
