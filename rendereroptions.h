#ifndef RENDEREROPTIONS_H
#define RENDEREROPTIONS_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>

// -----------------------------------------------------------------------------

struct RendererParams
{
    unsigned int    m_numSamples;
    unsigned int    m_numEmittedGlobalPhotons;
    unsigned int    m_numEmittedCausticPhotons;
    int             m_numAssociatedPhotons;
    float           m_streamRadius;
    float           m_globalSearchingRadius;
    unsigned int    m_numSearchedGlobalPhotons;
    float           m_causticSearchingRadius;
    unsigned int    m_numSearchedCausticPhotons;
    int             m_numPhotons;
    int             m_maxReflections;
};

// -----------------------------------------------------------------------------

class StreamRendererOptions : public QWidget
{
    Q_OBJECT
public:
    explicit StreamRendererOptions(QWidget *parent = 0);

    RendererParams getRendererParams() const;

private:
    QSpinBox*       m_numSamples;
    QSpinBox*       m_numEmittedGlobalPhotons;
    QSpinBox*       m_numEmittedCausticPhotons;
    QSpinBox*       m_numAssociatedPhotons;
    QDoubleSpinBox* m_streamRadius;
    QDoubleSpinBox* m_globalSearchingRadius;
    QSpinBox*       m_numSearchedGlobalPhotons;
    QDoubleSpinBox* m_causticSearchingRadius;
    QSpinBox*       m_numSearchedCausticPhotons;
    QSpinBox*       m_maxReflections;
};

// -----------------------------------------------------------------------------

class PhotonMapRendererOptions : public QWidget
{
    Q_OBJECT
public:
    explicit PhotonMapRendererOptions(QWidget *parent = 0);

    RendererParams getRendererParams() const;

private:
    QSpinBox*       m_numSamples;
    QSpinBox*       m_numEmittedGlobalPhotons;
    QSpinBox*       m_numEmittedCausticPhotons;
};

// -----------------------------------------------------------------------------

class PhotonMapVisualizerOptions : public QWidget
{
    Q_OBJECT
public:
    explicit PhotonMapVisualizerOptions(QWidget *parent = 0);

    RendererParams getRendererParams() const;

private:
    QSpinBox*       m_numPhotons;
    QSpinBox*       m_maxReflections;
};

// -----------------------------------------------------------------------------

class StreamVisualizerOptions : public QWidget
{
    Q_OBJECT
public:
    explicit StreamVisualizerOptions(QWidget *parent = 0);

    RendererParams getRendererParams() const;

private:
    QSpinBox*       m_numAssociatedPhotons;
    QDoubleSpinBox* m_streamRadius;
    QSpinBox*       m_numPhotons;
    QSpinBox*       m_maxReflections;
};

// -----------------------------------------------------------------------------

#endif // RENDEREROPTIONS_H
