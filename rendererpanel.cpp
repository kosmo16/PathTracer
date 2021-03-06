#include "rendererpanel.h"

RendererPanel::RendererPanel(QWidget *parent)
    : QWidget(parent)
    , m_rendererName(new QComboBox(this))
    , m_rendererOptionsPanel(new QStackedWidget(this))
    , m_streamRendererOptions(new StreamRendererOptions(this))
    , m_photonMapRendererOptions(new PhotonMapRendererOptions(this))
    , m_streamVisualizerOptions(new StreamVisualizerOptions(this))
    , m_photonMapVisualizerOptions(new PhotonMapVisualizerOptions)
    , m_timeLabel(new QLabel("",this))
{
    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    m_timeLabel->setFont(font);

    // Layout stuff
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_rendererName);
    layout->addWidget(m_rendererOptionsPanel);

    layout->addSpacerItem(new QSpacerItem(1,20,
                                          QSizePolicy::Fixed,
                                          QSizePolicy::Fixed));

    layout->addWidget(m_timeLabel);

    layout->addSpacerItem(new QSpacerItem(1,1,
                                          QSizePolicy::Fixed,
                                          QSizePolicy::Expanding));

    // Setup widgets
    m_rendererName->addItem("Stream Rendering");
    m_rendererName->addItem("Photon Map Rendering");
    m_rendererName->addItem("Stream Visualization");
    m_rendererName->addItem("Photon Map Visualization");

    m_rendererOptionsPanel->addWidget(m_streamRendererOptions);
    m_rendererOptionsPanel->addWidget(m_photonMapRendererOptions);
    m_rendererOptionsPanel->addWidget(m_streamVisualizerOptions);
    m_rendererOptionsPanel->addWidget(m_photonMapVisualizerOptions);

    connect(m_rendererName,SIGNAL(currentIndexChanged(int)),
            this,SLOT(onCurrentChanged(int)));
}

void RendererPanel::onCurrentChanged(int index)
{
    m_rendererOptionsPanel->setCurrentIndex(index);
}

RendererParams RendererPanel::getRendererParams() const
{
    RendererParams params;

    if (StreamRenderer == m_rendererName->currentIndex()) {
        params = m_streamRendererOptions->getRendererParams();
    } else if (PhotonMapRenderer == m_rendererName->currentIndex()) {
        params = m_photonMapRendererOptions->getRendererParams();
    } else if (StreamVisualizer == m_rendererName->currentIndex()) {
        params = m_streamVisualizerOptions->getRendererParams();
    } else if (PhotonMapVisualizer == m_rendererName->currentIndex()) {
        params = m_photonMapVisualizerOptions->getRendererParams();
    }
    return params;
}

RenderingMethod RendererPanel::renderingMethod() const
{
    return static_cast<RenderingMethod>(m_rendererName->currentIndex());
}

void RendererPanel::updateRenderingTime(int time)
{
    int milis = time % 1000;
    time /= 1000;
    int secs = time % 60;
    time /= 60;
    int mins = time % 60;
    time /= 60;
    int hours = time;
    if(hours != 0)
    {
        m_timeLabel->setText(QString("Rendering time: %1h %2min %3s %4 ms").arg(hours).arg(mins).arg(secs).arg(milis));
    }
    else if(mins != 0)
    {
        m_timeLabel->setText(QString("Rendering time: %1min %2s %3ms").arg(mins).arg(secs).arg(milis));
    }
    else if(secs != 0)
    {
        m_timeLabel->setText(QString("Rendering time: %1s %2ms").arg(secs).arg(milis));
    }
    else
    {
        m_timeLabel->setText(QString("Rendering time: %1ms").arg(milis));
    }
}
