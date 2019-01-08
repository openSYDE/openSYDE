//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Handle project save and load

   Handle project save and load

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <cstdio>
#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLDateTime.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "C_OSCProjectFiler.h"
#include "C_OSCXMLParser.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Save project

   Save project data to XML file.
   The target path must exists.
   A potentially pre-existing file will be erased.

   Before saving the:
   - Author will be overwritten with the name of the user logged into the PC system
   - ModificationTime will be overwritten with the current time

   \param[in,out]  orc_Project         Project data to save (Author and ModificationTime will be updated)
   \param[in]      orc_Path            Path of project
   \param[in]      orc_OpenSYDEVersion Current openSYDE version

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (could not erase pre-existing file before saving;
               no write access to file)
   C_RANGE    orc_Path is empty

   \created     05.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProjectFiler::h_Save(C_OSCProject & orc_Project, const C_SCLString & orc_Path,
                                 const C_SCLString & orc_OpenSYDEVersion)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check if file was specified
   if (orc_Path == "")
   {
      s32_Retval = C_RANGE;
   }
   else
   {
      bool q_NewFile = true;
      //erase file if it already exists:
      if (TGL_FileExists(orc_Path) == true)
      {
         //erase it:
         sintn sn_Return;
         sn_Return = std::remove(orc_Path.c_str());
         if (sn_Return != 0)
         {
            osc_write_log_error("Saving project file", "Could not erase pre-existing file \"" + orc_Path + "\".");
            s32_Retval = C_RD_WR;
         }
         q_NewFile = false;
      }

      //Normal XML
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = m_SaveInternal(orc_Project, orc_Path, orc_OpenSYDEVersion, q_NewFile);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Saving project file", "Could not write to file \"" + orc_Path + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load project

   Load project data from XML file.

   \param[in,out] orc_Project Project data
   \param[in]     orc_Path    Path of project

   \return
   C_NO_ERR   data read
   C_RANGE    specified file does not exist
   C_NOACT    specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   content of file is invalid or incomplete

   \created     05.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProjectFiler::h_Load(C_OSCProject & orc_Project, const C_SCLString & orc_Path)
{
   sint32 s32_Retval;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_SCLString c_Tmp;
      //Open file
      C_OSCXMLParser c_XML;
      s32_Retval = c_XML.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         //Check if file and root node exists
         if (c_XML.SelectRoot() == "Project")
         {
            c_Tmp = c_XML.GetAttributeString("author");
            if (c_Tmp == "")
            {
               stw_tgl::TGL_GetSystemUserName(c_Tmp);
            }
            orc_Project.c_Author = c_Tmp;
            //Time
            {
               orc_Project.c_CreationTime =
                  C_OSCProject::h_GetTimeOfString(c_XML.GetAttributeString("creation_time"));
            }
            {
               orc_Project.c_ModificationTime =
                  C_OSCProject::h_GetTimeOfString(c_XML.GetAttributeString("modification_time"));
            }
            orc_Project.c_OpenSYDEVersion = c_XML.GetAttributeString("openSYDE_version");
            orc_Project.c_Template = c_XML.GetAttributeString("template");
            //Check Version
            if (c_XML.SelectNodeChild("Version") == "Version")
            {
               s32_Retval = C_NO_ERR;
               orc_Project.c_Version = c_XML.GetNodeContent();
               c_XML.SelectNodeParent();
            }
            else
            {
               osc_write_log_error("Loading project file", "XML node \"Version\" not found.");
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            osc_write_log_error("Loading project file", "XML node \"Project\" not found.");
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading project file", "File does not exist \"" + orc_Path + "\".");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save project

   Warning: no error handling (job of caller)

   Before saving the:
   - Author will be overwritten with the name of the user logged into the PC system
   - ModificationTime will be overwritten with the current time

   \param[in,out]  orc_Project         Project data to save (Author and ModificationTime will be updated)
   \param[in]      orc_Path            Path of project
   \param[in]      orc_OpenSYDEVersion Current openSYDE version
   \param[in]      oq_New              Flag if file is new

   \return
   C_NO_ERR   data was written
   C_NOACT    could not write to file

   \created     06.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProjectFiler::m_SaveInternal(C_OSCProject & orc_Project, const C_SCLString & orc_Path,
                                         const C_SCLString & orc_OpenSYDEVersion, const bool oq_New)
{
   C_SCLString c_Tmp;
   //Open file
   C_OSCXMLParser c_XML;
   sint32 s32_Return;

   //create root:
   c_XML.CreateNodeChild("Project");
   c_XML.SelectRoot();

   //author
   stw_tgl::TGL_GetSystemUserName(c_Tmp);
   c_XML.SetAttributeString("author", c_Tmp);
   orc_Project.c_Author = c_Tmp;

   //Creation
   if (oq_New == true)
   {
      orc_Project.c_CreationTime = C_SCLDateTime::Now();
   }
   c_XML.SetAttributeString("creation_time", C_OSCProject::h_GetTimeFormatted(orc_Project.c_CreationTime));
   //modification
   orc_Project.c_ModificationTime = C_SCLDateTime::Now();
   c_XML.SetAttributeString("modification_time", C_OSCProject::h_GetTimeFormatted(orc_Project.c_ModificationTime));

   //openSYDE version
   orc_Project.c_OpenSYDEVersion = orc_OpenSYDEVersion;
   c_XML.SetAttributeString("openSYDE_version", orc_Project.c_OpenSYDEVersion);
   //Template
   c_XML.SetAttributeString("template", orc_Project.c_Template);

   //update version
   c_XML.CreateAndSelectNodeChild("Version");
   c_XML.SetNodeContent(orc_Project.c_Version);
   c_XML.SelectNodeParent();

   s32_Return = c_XML.SaveToFile(orc_Path);
   return s32_Return;
}
