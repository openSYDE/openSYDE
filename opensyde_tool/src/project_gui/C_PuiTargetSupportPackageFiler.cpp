//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Target support package UI side handling

   Target support package UI side handling

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscZipFile.hpp"
#include "C_OscNodeFiler.hpp"
#include "C_PuiSdHandlerFiler.hpp"
#include "C_OscTargetSupportPackageFiler.hpp"
#include "C_PuiTargetSupportPackageFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load tsp v3

   \param[in]      orc_TspPath   Tsp path
   \param[in,out]  orc_Tsp       Tsp
   \param[in,out]  orc_OscNode   Osc node
   \param[in,out]  orc_UiNode    Ui node

   \return
   C_NO_ERR    data read
   C_RANGE     specified target support package file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    in specified file is a XML node or attribute missing
   C_BUSY      Use different loader for this file version
   C_RD_WR     could not unpack archive to target path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiTargetSupportPackageFiler::h_LoadTspV3(const QString & orc_TspPath,
                                                    stw::opensyde_core::C_OscTargetSupportPackage & orc_Tsp,
                                                    stw::opensyde_core::C_OscNode & orc_OscNode,
                                                    C_PuiSdNode & orc_UiNode)
{
   stw::scl::C_SclString c_NodePath;
   int32_t s32_Retval = C_OscTargetSupportPackageFiler::h_Load(orc_Tsp, c_NodePath, orc_TspPath.toStdString().c_str());
   if (s32_Retval == C_NO_ERR)
   {
      QString c_FolderPath;
      const QString c_ZipFilePath =
         C_PuiTargetSupportPackageFiler::mh_GetNodeFilePath(orc_TspPath, c_NodePath.c_str());
      if (mh_DoUnzip(c_ZipFilePath, c_FolderPath) == C_NO_ERR)
      {
         const QString c_OscXmlPath = C_PuiTargetSupportPackageFiler::mh_GetOscNodeXmlPath(c_FolderPath);
         s32_Retval = C_OscNodeFiler::h_LoadNodeFile(orc_OscNode, c_OscXmlPath.toStdString().c_str());
         if (s32_Retval == C_NO_ERR)
         {
            const QString c_UiXmlPath = C_PuiTargetSupportPackageFiler::mh_GetUiNodeXmlPath(c_FolderPath);
            const QFileInfo c_FileInfo(c_UiXmlPath);
            const QDir c_Dir = c_FileInfo.dir();
            s32_Retval = C_PuiSdHandlerFiler::h_LoadNodeFile(orc_UiNode, c_UiXmlPath, &c_Dir, &orc_OscNode);
            if (s32_Retval == C_NO_ERR)
            {
               mh_DeleteFolder(c_FolderPath);
            }
         }
      }
      else
      {
         s32_Retval = C_RD_WR;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiTargetSupportPackageFiler::C_PuiTargetSupportPackageFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do unzip

   \param[in]      orc_ZipFilePath  Zip file path
   \param[in,out]  orc_FolderPath   Folder path

   \return
   C_NO_ERR    success
   C_RD_WR     could not unpack archive to target path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiTargetSupportPackageFiler::mh_DoUnzip(const QString & orc_ZipFilePath, QString & orc_FolderPath)
{
   stw::scl::C_SclString c_ErrorText;
   int32_t s32_Retval;

   orc_FolderPath = mh_GetZipExtractFolder();

   s32_Retval = mh_DeleteFolder(orc_FolderPath);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscZipFile::h_UnpackZipFile(
         orc_ZipFilePath.toStdString().c_str(), orc_FolderPath.toStdString().c_str(),
         &c_ErrorText);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node file path

   \param[in]  orc_TspPath    Tsp path
   \param[in]  orc_NodeFile   Node file

   \return
   Node file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiTargetSupportPackageFiler::mh_GetNodeFilePath(const QString & orc_TspPath, const QString & orc_NodeFile)
{
   const QFileInfo c_FileInfo(orc_TspPath);
   const QDir c_Dir = c_FileInfo.dir();

   return c_Dir.absoluteFilePath(orc_NodeFile);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get osc node xml path

   \param[in]  orc_NodeFolder    Node folder

   \return
   Osc node xml path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiTargetSupportPackageFiler::mh_GetOscNodeXmlPath(const QString & orc_NodeFolder)
{
   const QDir c_Dir(orc_NodeFolder);

   return c_Dir.absoluteFilePath(C_OscNodeFiler::h_GetFileName().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get ui node xml path

   \param[in]  orc_NodeFolder    Node folder

   \return
   Ui node xml path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiTargetSupportPackageFiler::mh_GetUiNodeXmlPath(const QString & orc_NodeFolder)
{
   const QDir c_Dir(orc_NodeFolder);

   return c_Dir.absoluteFilePath(C_PuiSdHandlerFiler::h_GetNodeUiFileName());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get zip extract folder

   \return
   Zip extract folder
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiTargetSupportPackageFiler::mh_GetZipExtractFolder()
{
   const QDir c_Dir(C_Uti::h_GetExePath());

   return c_Dir.absoluteFilePath("temp_tsp_extract");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete folder

   \param[in]  orc_Folder  Folder

   \return
   C_NO_ERR    success
   C_RD_WR     could not delete folder
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiTargetSupportPackageFiler::mh_DeleteFolder(const QString & orc_Folder)
{
   int32_t s32_Retval = C_NO_ERR;
   QDir c_CreateInFolder(orc_Folder);

   if (c_CreateInFolder.exists() == true)
   {
      if (c_CreateInFolder.removeRecursively() != true)
      {
         s32_Retval = C_RD_WR;
      }
   }
   return s32_Retval;
}
