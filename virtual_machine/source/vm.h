#ifndef _VM_H
#define _VM_H

#define ESP 0.000001
#define PRINT_INSTR 1
#ifndef TRUE
    #define TRUE                    1           // True
#endif

#ifndef FALSE
    #define FALSE                   0           // False
#endif
// ---- LoadScript () Error Codes ---------------------------------------------------------

#define LOAD_OK						0			// Load successful
#define LOAD_ERROR_FILE_IO  	    1			// File I/O error (most likely a file
// not found error
#define LOAD_ERROR_INVALID_XSE		2			// Invalid .XSE structure
#define LOAD_ERROR_UNSUPPORTED_VERS	3			// The format version is unsupported

#define XSE_ID_STRING               "XSE0"      // Used to validate an .XSE executable

// ---- Stack -----------------------------------------------------------------------------

#define DEF_STACK_SIZE			    1024	    // The default stack size



#endif
