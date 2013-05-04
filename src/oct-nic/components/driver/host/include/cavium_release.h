/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */

/*! \file cavium_release.h  
 *  \brief Routine to parse release string.
 */

 
#ifndef __CAVIUM_RELEASE_H__
#define __CAVIUM_RELEASE_H__

static inline void
cavium_parse_cvs_string(const char *cvs_name, char *ver_string, int len)
{
    static char version[80], cvs_name_str[80];
    char *ptr;

    /* The compiler starts complaining if cvs_name is used directly about
       array subscript exceeding boundary (since it doesnt know size of
       cvs_name??) , so copy locally. */
    strcpy(cvs_name_str, cvs_name);

    /* Character 7 is a space when there isn't a tag. Use this as a key to
       return the build date */
    if (strlen(cvs_name_str) < 7 || cvs_name_str[7] == ' ')
    {
        snprintf(version, sizeof(version), "Internal %s", __DATE__);
        version[sizeof(version)-1] = 0;
        strcpy(ver_string, version);
    }
    else
    {
        /* Make a static copy of the CVS Name string so we can modify it */
        strncpy(version, cvs_name_str, sizeof(version));
        version[sizeof(version)-1] = 0;

        /* Make sure there is an ending space in case someone didn't pass us
           a CVS Name string */
        version[sizeof(version)-2] = ' ';

        /* Truncate the string at the first space after the tag */
        *strchr(version+7, ' ') = 0;

        /* Convert all underscores into spaces or dots */
        while ((ptr = strchr(version, '_')) != NULL)
        {
            if ((ptr == version) ||                     /* Assume an underscore at beginning should be a space */
                (ptr[-1] < '0') || (ptr[-1] > '9') ||   /* If the character before it isn't a digit */
                (ptr[1] < '0') || (ptr[1] > '9'))       /* If the character after it isn't a digit */
                *ptr = ' ';
            else
                *ptr = '.';
        }

        /* Skip over the dollar Name: at the front */
	strcpy(ver_string, (version + 7));
    }

}


#endif
 
/* $Id: cavium_release.h 42868 2009-05-19 23:57:52Z panicker $ */
