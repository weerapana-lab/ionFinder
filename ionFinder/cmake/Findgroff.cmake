
find_program(GROFF groff)

if(GROFF MATCHES GROFF-NOTFOUND)
    set(GROFF_FOUND FALSE)
    set(GROFF_NOT_FOUND TRUE)
else()
    set(GROFF_FOUND TRUE)
    set(GROFF_NOT_FOUND FALSE)
endif()
