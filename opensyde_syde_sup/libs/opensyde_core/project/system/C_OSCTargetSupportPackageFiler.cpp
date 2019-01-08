//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief        File handler for target support package file data.

   Handle XML loading and parsing.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.10.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCTargetSupportPackageFiler.h"
#include "TGLFile.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCXMLParser.h"
#include "CSCLString.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Load target support package.

   Load data from specified file and place it in target support package instance

   \param[out]    orc_TargetSupportPackage   target support package information read from file
   \param[in]     orc_Path                   path to file

   \return
   C_NO_ERR    data read
   C_RANGE     specified target support package file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    in specified file is a XML node or attribute missing

   \created     01.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
sint32 C_OSCTargetSupportPackageFiler::h_Load(C_OSCTargetSupportPackage & orc_TargetSupportPackage,
                                              const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCXMLParser c_XMLParser;

      s32_Return = c_XMLParser.LoadFromFile(orc_Path);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = mh_Load(orc_TargetSupportPackage, c_XMLParser);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Load data from xml file.

   Parse a .syde_tsp file and save parameters to C_OSCTargetSupportPackage.

   \param[in]     orc_TargetSupportPackage      target support package information to write to file
   \param[in]     orc_XMLParser                 XML parser

   \return
   C_NO_ERR    XML data read and placed into target support package instance
   C_CONFIG    XML node or attribute missing

   \created     01.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
sint32 C_OSCTargetSupportPackageFiler::mh_Load(C_OSCTargetSupportPackage & orc_TargetSupportPackage,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;
   C_SCLString c_Text;

   //"empty" target support package to have a clearly defined status:
   orc_TargetSupportPackage.Clear();

   //Check if root node exists:
   c_Text = orc_XMLParser.SelectRoot();

   if (c_Text != "opensyde-target-support-package")
   {
      osc_write_log_error("Loading target support package", "XML node \"opensyde-target-support-package\" not found.");
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Text = orc_XMLParser.SelectNodeChild("file-version");
      if (c_Text != "file-version")
      {
         osc_write_log_error("Loading target support package", "XML node \"file-version\" not found.");
         s32_Return = C_CONFIG;
      }
      //no special handling required yet based on version
      orc_XMLParser.SelectNodeParent(); //back to parent
   }

   // device name
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("device-name");
      if (c_Text != "device-name")
      {
         osc_write_log_error("Loading target support package", "XML node \"device-name\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         orc_TargetSupportPackage.c_DeviceName = orc_XMLParser.GetNodeContent();
      }
      orc_XMLParser.SelectNodeParent(); //back to parent
   }

   // TSP comment
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("tsp-comment");
      if (c_Text != "tsp-comment")
      {
         osc_write_log_error("Loading target support package", "XML node \"tsp-comment\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         orc_TargetSupportPackage.c_Comment = orc_XMLParser.GetNodeContent();
      }
      orc_XMLParser.SelectNodeParent();
   }

   //openSYDE server settings
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("open-syde-server-settings");
      if (c_Text != "open-syde-server-settings")
      {
         osc_write_log_error("Loading target support package", "XML node \"open-syde-server-settings\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         if (orc_XMLParser.AttributeExists("dpd-application-index") == true)
         {
            orc_TargetSupportPackage.u8_ApplicationIndex =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("dpd-application-index"));
         }
         else
         {
            osc_write_log_error("Loading target support package", "XML attribute \"dpd-application-index\" not found "
                                "in node \"open-syde-server-setting\".");
            s32_Return = C_CONFIG;
         }
         if ((s32_Return == C_NO_ERR) && (orc_XMLParser.AttributeExists("max-parallel-transmissions") == true))
         {
            orc_TargetSupportPackage.u8_MaxParallelTransmissions =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("max-parallel-transmissions"));
         }
         else
         {
            osc_write_log_error("Loading target support package", "XML attribute \"max-parallel-transmissions\" not found "
                                "in node \"open-syde-server-setting\".");
            s32_Return = C_CONFIG;
         }
         if ((s32_Return == C_NO_ERR) && (orc_XMLParser.AttributeExists("max-tx-message-buffer") == true))
         {
            orc_TargetSupportPackage.u16_MaxMessageBufferTx =
               static_cast<uint16>(orc_XMLParser.GetAttributeUint32("max-tx-message-buffer"));
         }
         else
         {
            osc_write_log_error("Loading target support package", "XML attribute \"max-tx-message-buffer\" not found "
                                "in node \"open-syde-server-setting\".");
            s32_Return = C_CONFIG;
         }
         if ((s32_Return == C_NO_ERR) && (orc_XMLParser.AttributeExists("max-rx-routing-message-buffer") == true))
         {
            orc_TargetSupportPackage.u16_MaxRoutingMessageBufferRx =
               static_cast<uint16>(orc_XMLParser.GetAttributeUint32("max-rx-routing-message-buffer"));
         }
         else
         {
            osc_write_log_error("Loading target support package", "XML attribute \"max-rx-routing-message-buffer\" not found "
                                "in node \"open-syde-server-setting\".");
            s32_Return = C_CONFIG;
         }
      }

      //Return
      orc_XMLParser.SelectNodeParent();
   }

   // template project
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("template-project");
      if (c_Text != "template-project")
      {
         osc_write_log_error("Loading target support package", "XML node \"template-project\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         //template path
         c_Text = orc_XMLParser.SelectNodeChild("template");
         if (c_Text != "template")
         {
            osc_write_log_error("Loading target support package", "No XML node \"template\" found.");
            s32_Return = C_CONFIG;
         }
         else
         {
            orc_TargetSupportPackage.c_TemplatePath = orc_XMLParser.GetNodeContent();
         }
      }
      orc_XMLParser.SelectNodeParent();
   }

   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("application");
      if (c_Text != "application")
      {
         osc_write_log_error("Loading target support package", "XML node \"application\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         // parse this application
         do
         {
            s32_Return = mh_ParseApplication(orc_TargetSupportPackage, orc_XMLParser);
            c_Text = orc_XMLParser.SelectNodeNext("application");
         }
         while ((s32_Return == C_NO_ERR) && (c_Text == "application"));
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load applications data from XML file.

   Requires the XML parser to be on node "application".

   \param[in]     orc_TargetSupportPackage      target support package information to write to file
   \param[in]     orc_XMLParser                 XML parser

   \return
   C_NO_ERR    application data read and placed into target support package instance
   C_CONFIG    XML node or attribute missing

   \created     02.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
sint32 C_OSCTargetSupportPackageFiler::mh_ParseApplication(C_OSCTargetSupportPackage & orc_TargetSupportPackage,
                                                           C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;
   C_SCLString c_Text;
   C_OSCTSPApplication c_Application;

   // process id
   if (orc_XMLParser.AttributeExists("process-id") == false)
   {
      osc_write_log_error("Loading target support package", "XML attribute \"process-id\" not found.");
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Application.u8_ProcessId = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("process-id"));
   }

   // display name of application
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("display-name");
      if (c_Text != "display-name")
      {
         osc_write_log_error("Loading target support package", "No XML node \"display-name\" found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         c_Application.c_Name = orc_XMLParser.GetNodeContent();
      }
      orc_XMLParser.SelectNodeParent(); //back to parent
   }

   // comment
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("comment");
      if (c_Text != "comment")
      {
         osc_write_log_error("Loading target support package", "No XML node \"comment\" found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         c_Application.c_Comment = orc_XMLParser.GetNodeContent();
      }
      orc_XMLParser.SelectNodeParent(); //back to parent
   }

   // IDE call
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("ide-call");
      if (c_Text != "ide-call")
      {
         osc_write_log_error("Loading target support package", "XML node \"ide-call\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         c_Application.c_IdeCall = orc_XMLParser.GetNodeContent();
      }
      orc_XMLParser.SelectNodeParent(); //back to parent
   }

   // code generation
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("code-generation");
      if (c_Text != "code-generation")
      {
         osc_write_log_error("Loading target support package", "XML node \"code-generation\" not found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         // code generation type
         c_Text = orc_XMLParser.SelectNodeChild("type");
         if (c_Text != "type")
         {
            osc_write_log_error("Loading target support package",
                                "XML node \"code-generation\".\"type\" not found.");
            s32_Return = C_CONFIG;
         }
         else
         {
            if (orc_XMLParser.AttributeExists("is-standard-opensyde") == false)
            {
               osc_write_log_error("Loading target support package",
                                   "XML attribute \"is-standard-opensyde\" not found.");
               s32_Return = C_CONFIG;
            }
            else
            {
               c_Application.q_IsStandardSydeCoderC = orc_XMLParser.GetAttributeBool("is-standard-opensyde");
            }
         }
         orc_XMLParser.SelectNodeParent(); //back to parent

         // if the code generator is not standard, a path do the code generator is required
         if ((s32_Return == C_NO_ERR) && (c_Application.q_IsStandardSydeCoderC == false))
         {
            c_Text = orc_XMLParser.SelectNodeChild("path");
            if (c_Text != "path")
            {
               osc_write_log_error("Loading target support package",
                                   "XML node \"code-generation\".\"path\" not found but required.");
               s32_Return = C_CONFIG;
            }
            else
            {
               c_Application.c_CodeGeneratorPath = orc_XMLParser.GetNodeContent();
            }
            orc_XMLParser.SelectNodeParent(); //back to parent
         }
      }
      tgl_assert(orc_XMLParser.SelectNodeParent() == "application"); //back to parent
   }

   // project-folder
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("project-folder");
      if (c_Text != "project-folder")
      {
         osc_write_log_error("Loading target support package", "No XML node \"project-folder\" found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         c_Application.c_ProjectFolder = orc_XMLParser.GetNodeContent();
      }
      orc_XMLParser.SelectNodeParent(); //back to parent
   }

   // generate
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("generate");
      if (c_Text != "generate")
      {
         osc_write_log_error("Loading target support package", "No XML node \"generate\" found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         c_Application.c_GeneratePath = orc_XMLParser.GetNodeContent();
      }
      orc_XMLParser.SelectNodeParent(); //back to parent
   }

   // result
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_XMLParser.SelectNodeChild("result");
      if (c_Text != "result")
      {
         osc_write_log_error("Loading target support package", "No XML node \"result\" found.");
         s32_Return = C_CONFIG;
      }
      else
      {
         c_Application.c_ResultPath = orc_XMLParser.GetNodeContent();
      }
      orc_XMLParser.SelectNodeParent(); //back to parent
   }

   // add application
   orc_TargetSupportPackage.c_Applications.push_back(c_Application);

   return s32_Return;
}
