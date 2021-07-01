//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for all connected node entries (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCCONNECTEDNODELIST_H
#define C_SYVDCCONNECTEDNODELIST_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QListWidget>
#include "stwtypes.h"
#include "C_SyvDcSequences.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDcConnectedNodeList :
   public QListWidget
{
   Q_OBJECT

public:
   C_SyvDcConnectedNodeList(QWidget * const opc_Parent = NULL);

   void SetData(const std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceInformation> & orc_Infos);
   void EnableSerialNumber(const QString & orc_SerialNumber) const;
   void DisableSerialNumber(const QString & orc_SerialNumber) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid);
   void SigStopDrag(void);

protected:
   virtual void startDrag(const Qt::DropActions oc_Actions) override;
   virtual QMimeData * mimeData(const QList<QListWidgetItem *> oc_Items) const override;

private:
   bool mq_GridSizeSet;
   std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceInformation> mc_Data;
   static const QString mhc_MimeData;
   static const QString mhc_MimeDataDevice;
   static const QString mhc_MimeDataDeviceValid;

   void m_Init(void);
   void m_AppendNode(const stw_opensyde_gui_logic::C_SyvDcDeviceInformation & orc_Info);

   void m_ScrollBarRangeChangedVer(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
