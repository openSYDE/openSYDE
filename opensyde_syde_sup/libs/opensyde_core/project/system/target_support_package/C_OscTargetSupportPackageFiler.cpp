//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       File handler for target support package file data.

   Handle XML loading and parsing.

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscTargetSupportPackageFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load target support package.

   Load data from specified file and place it in target support package instance

   \param[out]     orc_TargetSupportPackage  target support package information read from file
   \param[in,out]  orc_NodePath              Node path
   \param[in]      orc_Path                  path to file

   \return
   C_NO_ERR    data read
   C_RANGE     specified target support package file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    in specified file is a XML node or attribute missing
   C_BUSY      Use different loader for this file version
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscTargetSupportPackageFiler::h_Load(C_OscTargetSupportPackage & orc_TargetSupportPackage,
                                               stw::scl::C_SclString & orc_NodePath,
                                               const stw::scl::C_SclString & orc_Path)
{
   int32_t s32_Return = C_NO_ERR;

   if (TglFileExists(orc_Path) == true)
   {
      C_OscXmlParser c_XmlParser;

      s32_Return = c_XmlParser.LoadFromFile(orc_Path);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = mh_Load(orc_TargetSupportPackage, orc_NodePath, c_XmlParser);
      }
      else
      {
         osc_write_log_error("Loading Target Support Package",
                             "File \"" + orc_Path + "\" could not be opened.");
         s32_Return = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading Target Support Package", "File \"" + orc_Path + "\" does not exist.");
      s32_Return = C_RANGE;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from xml file.

   Parse a .syde_tsp file and save parameters to C_OscTargetSupportPackage.

   \param[in]      orc_TargetSupportPackage  target support package information to write to file
   \param[in,out]  orc_NodePath              Node path
   \param[in]      orc_XmlParser             XML parser

   \return
   C_NO_ERR    XML data read and placed into target support package instance
   C_CONFIG    XML node or attribute missing
   C_BUSY      Use different loader for this file version
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscTargetSupportPackageFiler::mh_Load(C_OscTargetSupportPackage & orc_TargetSupportPackage,
                                                stw::scl::C_SclString & orc_NodePath,
                                                C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return = C_NO_ERR;

   stw::scl::C_SclString c_Text;

   //"empty" target support package to have a clearly defined status:
   orc_TargetSupportPackage.Clear();

   //Check if root node exists:
   c_Text = orc_XmlParser.SelectRoot();

   if (c_Text != "opensyde-target-support-package")
   {
      osc_write_log_error("Loading target support package", "XML node \"opensyde-target-support-package\" not found.");
      s32_Return = C_CONFIG;
   }
   else
   {
      if (orc_XmlParser.SelectNodeChild("file-version") == "file-version")
      {
         uint16_t u16_FileVersion = 0U;
         try
         {
            u16_FileVersion = static_cast<uint16_t>(orc_XmlParser.GetNodeContent().ToInt());
         }
         catch (...)
         {
            osc_write_log_error("Loading target support package",
                                "\"file-version\" could not be converted to a number.");
            s32_Return = C_CONFIG;
         }

         //is the file version one we know ?
         if (s32_Return == C_NO_ERR)
         {
            osc_write_log_info("Loading target support package", "Value of \"file-version\": " +
                               stw::scl::C_SclString::IntToStr(u16_FileVersion));
            //Check file version
            if ((u16_FileVersion != 3U))
            {
               if ((u16_FileVersion != 1U) && (u16_FileVersion != 2U))
               {
                  osc_write_log_error("Loading target support package",
                                      "Version defined by \"file-version\" is not supported.");
                  s32_Return = C_CONFIG;
               }
               else
               {
                  s32_Return = C_BUSY;
               }
            }
         }
      }
      else
      {
         osc_write_log_error("Loading target support package", "XML node \"file-version\" not found.");
         s32_Return = C_CONFIG;
      }
      //no special handling required yet based on version
      orc_XmlParser.SelectNodeParent(); //back to parent
   }

   // device name
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XmlParser.SelectNodeChild("device-name");
      if (c_Text != "device-name")
      {
         osc_write_log_error("Loading target support package", "XML node \"device-name\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         orc_TargetSupportPackage.c_DeviceName = orc_XmlParser.GetNodeContent();
      }
      orc_XmlParser.SelectNodeParent(); //back to parent
   }

   // TSP comment
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XmlParser.SelectNodeChild("tsp-comment");
      if (c_Text != "tsp-comment")
      {
         osc_write_log_error("Loading target support package", "XML node \"tsp-comment\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         orc_TargetSupportPackage.c_Comment = orc_XmlParser.GetNodeContent();
      }
      orc_XmlParser.SelectNodeParent();
   }

   // Node definition
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XmlParser.SelectNodeChild("node-definition");
      if (c_Text != "node-definition")
      {
         osc_write_log_error("Loading target support package", "XML node \"node-definition\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         orc_NodePath = orc_XmlParser.GetNodeContent();
      }
      orc_XmlParser.SelectNodeParent();
   }

   // template project
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XmlParser.SelectNodeChild("template-project");
      if (c_Text != "template-project")
      {
         //optional
         orc_TargetSupportPackage.c_TemplatePath = "";
      }
      else
      {
         //template path
         c_Text = orc_XmlParser.SelectNodeChild("template");
         if (c_Text != "template")
         {
            //optional
            orc_TargetSupportPackage.c_TemplatePath = "";
         }
         else
         {
            orc_TargetSupportPackage.c_TemplatePath = orc_XmlParser.GetNodeContent();
         }
      }
      orc_XmlParser.SelectNodeParent();
   }

   return s32_Return;
}
