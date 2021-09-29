# for classes_def.xml!!

cet_make( LIBRARIES
	  	    ${ROOT_EG}
		    NO_DICTIONARY
)

art_dictionary( DICTIONARY_LIBRARIES ubobj_Trigger )

install_headers()
install_fhicl()
install_source()
