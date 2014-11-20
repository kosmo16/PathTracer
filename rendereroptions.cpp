#include "rendereroptions.h"

#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>

// -----------------------------------------------------------------------------

StreamRendererOptions::StreamRendererOptions(QWidget *parent)
    : QWidget(parent)
    , m_numSamples(new QSpinBox(this))
    , m_numEmittedGlobalPhotons(new QSpinBox(this))
    , m_numEmittedCausticPhotons(new QSpinBox(this))
    , m_numAssociatedPhotons(new QSpinBox(this))
    , m_streamRadius(new QDoubleSpinBox(this))
    , m_globalSearchingRadius(new QDoubleSpinBox(this))
    , m_numSearchedGlobalPhotons(new QSpinBox(this))
    , m_causticSearchingRadius(new QDoubleSpinBox(this))
    , m_numSearchedCausticPhotons(new QSpinBox(this))
    , m_maxReflections(new QSpinBox(this))
{
    // setup input widgets
    m_numSamples->setMinimum(0);
    m_numSamples->setMaximum(100000);
    m_numSamples->setValue(1);

    m_numEmittedGlobalPhotons->setMinimum(0);
    m_numEmittedGlobalPhotons->setMaximum(2000000);
    m_numEmittedGlobalPhotons->setValue(1000);

    m_numEmittedCausticPhotons->setMinimum(0);
    m_numEmittedCausticPhotons->setMaximum(100000);
    m_numEmittedCausticPhotons->setValue(0);

    m_numAssociatedPhotons->setMinimum(0);
    m_numAssociatedPhotons->setMaximum(100000);
    m_numAssociatedPhotons->setValue(10);

    m_streamRadius->setMinimum(0.001);
    m_streamRadius->setMaximum(10000.0);
    m_streamRadius->setValue(5.0);

    m_globalSearchingRadius->setMinimum(0.001);
    m_globalSearchingRadius->setMaximum(10000.0);
    m_globalSearchingRadius->setValue(3.0);

    m_numSearchedGlobalPhotons->setMinimum(0);
    m_numSearchedGlobalPhotons->setMaximum(100000);
    m_numSearchedGlobalPhotons->setValue(100);

    m_causticSearchingRadius->setMinimum(0.001);
    m_causticSearchingRadius->setMaximum(10000.0);
    m_causticSearchingRadius->setValue(1.0);

    m_numSearchedCausticPhotons->setMinimum(0);
    m_numSearchedCausticPhotons->setMaximum(100000);
    m_numSearchedCausticPhotons->setValue(100);

    m_maxReflections->setMinimum(0);
    m_maxReflections->setMaximum(100000);
    m_maxReflections->setValue(4);

    // layout them
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(new QLabel("Number of samples"),0,0);
    layout->addWidget(m_numSamples,0,1);
    layout->addWidget(new QLabel("Number of emitted photon streams"),1,0);
    layout->addWidget(m_numEmittedGlobalPhotons,1,1);
    layout->addWidget(new QLabel("Number of emitted caustic photons"),2,0);
    layout->addWidget(m_numEmittedCausticPhotons,2,1);
    layout->addWidget(new QLabel("Number of associated photons"),3,0);
    layout->addWidget(m_numAssociatedPhotons,3,1);
    layout->addWidget(new QLabel("Radius of photon stream"),4,0);
    layout->addWidget(m_streamRadius,4,1);
    layout->addWidget(new QLabel("Stream map searching radius"),5,0);
    layout->addWidget(m_globalSearchingRadius,5,1);
    layout->addWidget(new QLabel("Number of searched photon streams"),6,0);
    layout->addWidget(m_numSearchedGlobalPhotons,6,1);
    layout->addWidget(new QLabel("Caustic map searching radius"),7,0);
    layout->addWidget(m_causticSearchingRadius,7,1);
    layout->addWidget(new QLabel("Number of searched caustic photons"),8,0);
    layout->addWidget(m_numSearchedCausticPhotons,8,1);
    layout->addWidget(new QLabel("Number of stream reflections"),9,0);
    layout->addWidget(m_maxReflections,9,1);

    layout->addItem(new QSpacerItem(1,1,
                                    QSizePolicy::Fixed,
                                    QSizePolicy::Expanding),10,0,1,2);
}

RendererParams StreamRendererOptions::getRendererParams() const
{
    RendererParams params;

    params.m_numSamples             = m_numSamples->value();
    params.m_numEmittedGlobalPhotons    = m_numEmittedGlobalPhotons->value();
    params.m_numEmittedCausticPhotons   = m_numEmittedCausticPhotons->value();
    params.m_numAssociatedPhotons   = m_numAssociatedPhotons->value();
    params.m_streamRadius           = m_streamRadius->value();
    params.m_globalSearchingRadius  = m_globalSearchingRadius->value();
    params.m_numSearchedGlobalPhotons     = m_numSearchedGlobalPhotons->value();
    params.m_causticSearchingRadius = m_causticSearchingRadius->value();
    params.m_maxReflections = m_maxReflections->value();

    return params;
}

// -----------------------------------------------------------------------------

PhotonMapRendererOptions::PhotonMapRendererOptions(QWidget *parent)
    : QWidget(parent)
    , m_numSamples(new QSpinBox(this))
    , m_numEmittedGlobalPhotons(new QSpinBox(this))
    , m_numEmittedCausticPhotons(new QSpinBox(this))
{
    // setup input widgets
    m_numSamples->setMinimum(0);
    m_numSamples->setMaximum(100000);
    m_numSamples->setValue(1);

    m_numEmittedGlobalPhotons->setMinimum(0);
    m_numEmittedGlobalPhotons->setMaximum(1000000);
    m_numEmittedGlobalPhotons->setValue(120); //default 12000

    m_numEmittedCausticPhotons->setMinimum(0);
    m_numEmittedCausticPhotons->setMaximum(1000000);
    m_numEmittedCausticPhotons->setValue(5); //default 5000

    // layout them
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(new QLabel("Number of samples"),0,0);
    layout->addWidget(m_numSamples,0,1);
    layout->addWidget(new QLabel("Number of global map photons"),1,0);
    layout->addWidget(m_numEmittedGlobalPhotons,1,1);
    layout->addWidget(new QLabel("Number of caustic map photons"),2,0);
    layout->addWidget(m_numEmittedCausticPhotons,2,1);

    layout->addItem(new QSpacerItem(1,1,
                                    QSizePolicy::Fixed,
                                    QSizePolicy::Expanding),3,0,1,2);
}

RendererParams PhotonMapRendererOptions::getRendererParams() const
{
    RendererParams params;

    params.m_numSamples             = m_numSamples->value();
    params.m_numEmittedGlobalPhotons    = m_numEmittedGlobalPhotons->value();
    params.m_numEmittedCausticPhotons   = m_numEmittedCausticPhotons->value();

    return params;
}

// -----------------------------------------------------------------------------

PhotonMapVisualizerOptions::PhotonMapVisualizerOptions(QWidget *parent)
    : QWidget(parent)
    , m_numPhotons(new QSpinBox(this))
    , m_maxReflections(new QSpinBox(this))
{
    // setup input widgets
    m_numPhotons->setMinimum(0);
    m_numPhotons->setMaximum(100000);
    m_numPhotons->setValue(25000);

    m_maxReflections->setMinimum(0);
    m_maxReflections->setMaximum(10000);
    m_maxReflections->setValue(6);

    // layout them
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(new QLabel("Number of photons"),0,0);
    layout->addWidget(m_numPhotons,0,1);
    layout->addWidget(new QLabel("Max number of reflections"),1,0);
    layout->addWidget(m_maxReflections,1,1);

    layout->addItem(new QSpacerItem(1,1,
                                    QSizePolicy::Fixed,
                                    QSizePolicy::Expanding),2,0,1,2);
}

RendererParams PhotonMapVisualizerOptions::getRendererParams() const
{
    RendererParams params;

    params.m_numPhotons             = m_numPhotons->value();
    params.m_maxReflections         = m_maxReflections->value();

    return params;
}

// -----------------------------------------------------------------------------

StreamVisualizerOptions::StreamVisualizerOptions(QWidget *parent)
    : QWidget(parent)
    , m_numPhotons(new QSpinBox(this))
    , m_maxReflections(new QSpinBox(this))
    , m_numAssociatedPhotons(new QSpinBox(this))
    , m_streamRadius(new QDoubleSpinBox(this))
{
    // setup input widgets
    m_numPhotons->setMinimum(0);
    m_numPhotons->setMaximum(100000);
    m_numPhotons->setValue(12000);

    m_maxReflections->setMinimum(0);
    m_maxReflections->setMaximum(10000);
    m_maxReflections->setValue(6);

    m_numAssociatedPhotons->setMinimum(-100000);
    m_numAssociatedPhotons->setMaximum(100000);
    m_numAssociatedPhotons->setValue(50);

    m_streamRadius->setMinimum(-10000.0);
    m_streamRadius->setMaximum(10000.0);
    m_streamRadius->setValue(0.5);

    // layout them
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(new QLabel("Number of photons"),0,0);
    layout->addWidget(m_numPhotons,0,1);
    layout->addWidget(new QLabel("Max number of reflections"),1,0);
    layout->addWidget(m_maxReflections,1,1);
    layout->addWidget(new QLabel("Number of associated photons"),2,0);
    layout->addWidget(m_numAssociatedPhotons,2,1);
    layout->addWidget(new QLabel("Radius of photon stream"),3,0);
    layout->addWidget(m_streamRadius,3,1);

    layout->addItem(new QSpacerItem(1,1,
                                    QSizePolicy::Fixed,
                                    QSizePolicy::Expanding),4,0,1,2);
}

RendererParams StreamVisualizerOptions::getRendererParams() const
{
    RendererParams params;

    params.m_numAssociatedPhotons   = m_numAssociatedPhotons->value();
    params.m_streamRadius           = m_streamRadius->value();
    params.m_numPhotons             = m_numPhotons->value();
    params.m_maxReflections         = m_maxReflections->value();

    return params;
}

// -----------------------------------------------------------------------------
