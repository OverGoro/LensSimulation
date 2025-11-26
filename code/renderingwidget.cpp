#include "renderingwidget.h"

#include <QPainter>

RenderingWidget::RenderingWidget(QWidget *parent)
    : QWidget(parent),
    imageLabel(new QLabel(this)),
    progressBar(new QProgressBar(this)) {

    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(imageLabel);
    layout->addWidget(progressBar);

    progressBar->setRange(0, 100);
    progressBar->setValue(0);

    connect(this, &RenderingWidget::imageSizeChanged, this, [this]() {
        updateScaledImage();
    });
    setLayout(layout);
}

QSize RenderingWidget::getImageWidgetSize() const {
    return imageLabel->size();
}

void RenderingWidget::setImage(const std::vector<Vec3>& buffer, int width, int height) {
    currentImage = QImage(width, height, QImage::Format_RGB32);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const Vec3& pixel = buffer[y * width + x];
            currentImage.setPixel(x, y, qRgb(pixel.x * 255, pixel.y * 255, pixel.z * 255));
        }
    }
    updateScaledImage();
}

void RenderingWidget::setProgress(double percent) {
    progressBar->setValue(percent);
}

void RenderingWidget::setPixelColor(int x, int y, const Vec3& color) {
    if (x >= 0 && x < currentImage.width() && y >= 0 && y < currentImage.height()) {
        currentImage.setPixel(x, y, qRgb(color.x * 255, color.y * 255, color.z * 255));
        updateScaledImage();
    }
}

void RenderingWidget::updateScaledImage() {
    if (!currentImage.isNull()) {
        imageLabel->setPixmap(QPixmap::fromImage(currentImage.scaled(imageLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    }
}

void RenderingWidget::resizeEvent(QResizeEvent* event) {
    updateScaledImage();
    emit imageSizeChanged(imageLabel->size());
    QWidget::resizeEvent(event);
}
