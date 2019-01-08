//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for an existing node entry (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.12.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDCEXISTINGNODEWIDGET_H
#define C_SYVDCEXISTINGNODEWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QListWidgetItem>
#include "stwtypes.h"
#include "C_SyvDcSequences.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SyvDcExistingNodeWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDcExistingNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcExistingNodeWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDcExistingNodeWidget(void);

   stw_types::sint32 SetIndex(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex,
                              const stw_types::sint32 os32_ItemIndex, QListWidgetItem * const opc_Item,
                              const bool oq_ShowAssignment);
   bool CompareIndex(const stw_types::uint32 ou32_NodeIndex) const;
   void ConnectSerialNumber(const QString & orc_SerialNumber) const;
   void DisconnectSerialNumber(const QString & orc_SerialNumber) const;
   bool IsAssigned(void) const;
   void AppendDeviceConfig(std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_Configs) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConnect(const stw_types::uint32 ou32_NodeIndex, const QString & orc_SerialNumber);
   void SigDisconnect(const stw_types::uint32 ou32_NodeIndex, const QString & orc_SerialNumber);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void dragLeaveEvent(QDragLeaveEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_SyvDcExistingNodeWidget * mpc_Ui;
   QListWidgetItem * mpc_ListWidgetItem;
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::sint32 ms32_ItemIndex;
   QString mc_DeviceName;
   static const QString mhc_MimeData;
   static const QString mhc_MimeDataDevice;

   stw_types::sint32 m_Init(void);
   void m_OnDisconnectRequest(const QString & orc_SerialNumber);

   //Avoid call
   C_SyvDcExistingNodeWidget(const C_SyvDcExistingNodeWidget &);
   C_SyvDcExistingNodeWidget & operator =(const C_SyvDcExistingNodeWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
