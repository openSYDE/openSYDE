//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for RTF file export (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.06.2018  STW/D.Pohl (copied and adapted from data pool list import)
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_RTFEXPORTWIDGET_H
#define C_RTFEXPORTWIDGET_H

#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "CSCLString.h"
#include "CSCLStringList.h"
#include "C_SdTopologyWidget.h"

namespace Ui
{
class C_RtfExportWidget;
}
/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_RtfExportWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_RtfExportWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_RtfExportWidget(void);

   stw_types::sint32 GetRtfPath(stw_scl::C_SCLString & orc_RtfPath) const;
   stw_types::sint32 GetCompanyName(stw_scl::C_SCLString & orc_CompanyName) const;
   stw_types::sint32 GetCompanyLogoPath(stw_scl::C_SCLString & orc_CompanyLogoPath) const;

   void SetRtfPath(const stw_scl::C_SCLString & orc_RtfPath);
   void SetCompanyName(const stw_scl::C_SCLString & orc_CompanyName) const;
   void SetCompanyLogoPath(const stw_scl::C_SCLString & orc_CompanyLogoPath);

   stw_types::sint32 ExportToRtf(const stw_scl::C_SCLString & orc_RtfPath, const stw_scl::C_SCLString & orc_CompanyName,
                                 const stw_scl::C_SCLString & orc_CompanyLogoPath,
                                 C_SdTopologyWidget * const opc_Widget, stw_scl::C_SCLStringList & orc_WarningMessages,
                                 stw_scl::C_SCLString & orc_ErrorMessage);

   void InitStaticNames(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

   class C_ExportXmlStructure
   {
   public:
      // Project
      stw_scl::C_SCLString c_Title;
      stw_scl::C_SCLString c_Name;
      stw_scl::C_SCLString c_Version;
      stw_scl::C_SCLString c_Created;
      stw_scl::C_SCLString c_Author;
      stw_scl::C_SCLString c_SysDefPath;
      stw_scl::C_SCLString c_DevicesIniPath;
      stw_scl::C_SCLString c_OutputPath;
      stw_scl::C_SCLString c_NetworkTopologyImage;
      // openSYDE
      stw_scl::C_SCLString c_OpenSydeVersion;
      // Company
      stw_scl::C_SCLString c_CompanyName;
      stw_scl::C_SCLString c_CompanyLogoPath;
   };

   stw_types::sint32 m_CheckSettings(void) const;

private:
   Ui::C_RtfExportWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   std::map<stw_scl::C_SCLString, stw_scl::C_SCLString> mc_NodeMapping;
   stw_scl::C_SCLStringList mc_Warnings;
   stw_scl::C_SCLString mc_Error;

   stw_scl::C_SCLString mc_RtfPath;
   stw_scl::C_SCLString mc_CompanyLogoPath;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_RtfPathClicked(void);
   void m_LogoPathClicked(void);
   void m_EditRtfPathFocusOn(void) const;
   void m_EditLogoPathFocusOn(void) const;
   void m_EditRtfPathFocusOff(void);
   void m_EditLogoPathFocusOff(void);

   stw_types::sint32 m_CreateConfigXml(const stw_scl::C_SCLString & orc_Path,
                                       const C_ExportXmlStructure & orc_ExportXmlStructure) const;

   //Avoid call
   C_RtfExportWidget(const C_RtfExportWidget &);
   C_RtfExportWidget & operator =(const C_RtfExportWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
