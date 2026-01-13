#pragma once

// Platform detection and common definitions
/*

Perhaps it's an unnecessary macro, but in my opinion,
it's better to simply create a macro for the separator once
than to check the platform every time in the code.
Perhaps it will be expanded in the future.
*/

#if _WIN32
    #define PATH_SEPARATOR "\\"
#elif __linux__
    #define PATH_SEPARATOR "/"

    /*
    Directory permissions for Linux
    0755 = rwxr-xr-x:
    7 (owner):  read (4) + write (2) + execute (1) = full access
    5 (group):  read (4) + execute (1) = read and enter directory
    5 (others): read (4) + execute (1) = read and enter directory
    
    This allows program to fully manage logs/, while others can only read
    */
    #define DIR_PERMISSIONS 0755
#endif
