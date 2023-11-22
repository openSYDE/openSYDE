//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle project save and load

   Handle project save and load

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstdio>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclDateTime.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "C_OscProjectFiler.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::scl;

using namespace stw::errors;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project

   Save project data to XML file.
   The target path must exists.
   A potentially pre-existing file will be erased.

   Before saving the:
   - Author will be overwritten with the name of the user logged into the PC system
   - ModificationTime will be overwritten with the current time

   \param[in,out]  orc_Project         Project data to save (Author and ModificationTime will be updated)
   \param[in]      orc_Path            Path of project
   \param[in]      orc_OpenSydeVersion Current openSYDE version

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (could not erase pre-existing file before saving;
               no write access to file)
   C_RANGE    orc_Path is empty
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProjectFiler::h_Save(C_OscProject & orc_Project, const C_SclString & orc_Path,
                                  const C_SclString & orc_OpenSydeVersion)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check if file was specified
   if (orc_Path == "")
   {
      s32_Retval = C_RANGE;
   }
   else
   {
      bool q_NewFile = true;
      //erase file if it already exists:
      if (TglFileExists(orc_Path) == true)
      {
         //erase it:
         int x_Return; //lint !e970 !e8080  //using type to match library interface
         x_Return = std::remove(orc_Path.c_str());
         if (x_Return != 0)
         {
            osc_write_log_error("Saving project file", "Could not erase pre-existing file \"" + orc_Path + "\".");
            s32_Retval = C_RD_WR;
         }
         q_NewFile = false;
      }

      //Normal XML
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_SaveInternal(orc_Project, orc_Path, orc_OpenSydeVersion, q_NewFile);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Saving project file", "Could not write to file \"" + orc_Path + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load project

   Load project data from XML file.

   \param[in,out] orc_Project Project data
   \param[in]     orc_Path    Path of project

   \return
   C_NO_ERR   data read
   C_RANGE    specified file does not exist
   C_NOACT    specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProjectFiler::h_Load(C_OscProject & orc_Project, const C_SclString & orc_Path)
{
   int32_t s32_Retval;

   if (TglFileExists(orc_Path) == true)
   {
      C_SclString c_Tmp;
      //Open file
      C_OscXmlParser c_Xml;
      s32_Retval = c_Xml.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         //Check if file and root node exists
         if (c_Xml.SelectRoot() == "Project")
         {
            //Author & Editor
            c_Tmp = c_Xml.GetAttributeString("author");
            if (c_Tmp == "")
            {
               stw::tgl::TglGetSystemUserName(c_Tmp);
            }
            orc_Project.c_Author = c_Tmp;
            c_Tmp = c_Xml.GetAttributeString("editor");
            if (c_Tmp == "")
            {
               // use author if last editor is empty (reason: prior openSYDE versions handled author as editor)
               c_Tmp = orc_Project.c_Author;
            }
            orc_Project.c_Editor = c_Tmp;
            //Time
            {
               orc_Project.c_CreationTime =
                  C_OscProject::h_GetTimeOfString(c_Xml.GetAttributeString("creation_time"));
            }
            {
               orc_Project.c_ModificationTime =
                  C_OscProject::h_GetTimeOfString(c_Xml.GetAttributeString("modification_time"));
            }
            orc_Project.c_OpenSydeVersion = c_Xml.GetAttributeString("openSYDE_version");
            orc_Project.c_Template = c_Xml.GetAttributeString("template");
            //Check Version
            if (c_Xml.SelectNodeChild("Version") == "Version")
            {
               s32_Retval = C_NO_ERR;
               orc_Project.c_Version = c_Xml.GetNodeContent();
               c_Xml.SelectNodeParent();
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project

   Warning: no error handling (job of caller)

   Before saving the:
   - Editor will be overwritten with the name of the user logged into the PC system
   - ModificationTime will be overwritten with the current time

   \param[in,out]  orc_Project         Project data to save (Author and ModificationTime will be updated)
   \param[in]      orc_Path            Path of project
   \param[in]      orc_OpenSydeVersion Current openSYDE version
   \param[in]      oq_New              Flag if file is new

   \return
   C_NO_ERR   data was written
   C_NOACT    could not write to file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProjectFiler::mh_SaveInternal(C_OscProject & orc_Project, const C_SclString & orc_Path,
                                           const C_SclString & orc_OpenSydeVersion, const bool oq_New)
{
   C_SclString c_Tmp;
   //Open file
   C_OscXmlParser c_Xml;
   int32_t s32_Return;

   //create root:
   c_Xml.CreateNodeChild("Project");
   c_Xml.SelectRoot();

   //author
   if (oq_New == true)
   {
      stw::tgl::TglGetSystemUserName(c_Tmp);
      orc_Project.c_Author = c_Tmp;
   }
   c_Xml.SetAttributeString("author", orc_Project.c_Author);

   //editor
   stw::tgl::TglGetSystemUserName(c_Tmp);
   orc_Project.c_Editor = c_Tmp;
   c_Xml.SetAttributeString("editor", orc_Project.c_Editor);

   //Creation
   if (oq_New == true)
   {
      orc_Project.c_CreationTime = C_SclDateTime::Now();
   }
   c_Xml.SetAttributeString("creation_time", C_OscProject::h_GetTimeFormatted(orc_Project.c_CreationTime));

   //modification
   orc_Project.c_ModificationTime = C_SclDateTime::Now();
   c_Xml.SetAttributeString("modification_time", C_OscProject::h_GetTimeFormatted(orc_Project.c_ModificationTime));

   //openSYDE version
   orc_Project.c_OpenSydeVersion = orc_OpenSydeVersion;
   c_Xml.SetAttributeString("openSYDE_version", orc_Project.c_OpenSydeVersion);

   //Template
   c_Xml.SetAttributeString("template", orc_Project.c_Template);

   //update version
   c_Xml.CreateAndSelectNodeChild("Version");
   c_Xml.SetNodeContent(orc_Project.c_Version);
   c_Xml.SelectNodeParent();

   s32_Return = c_Xml.SaveToFile(orc_Path);
   return s32_Return;
}
