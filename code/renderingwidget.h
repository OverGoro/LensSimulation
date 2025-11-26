#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QImage>
#include <QVBoxLayout>
#include <QLabel>
#include <vector>
#include "primitives.h"

class RenderingWidget : public QWidget {
    Q_OBJECT

public:
    explicit RenderingWidget(QWidget *parent = nullptr);

    QSize getImageWidgetSize() const;
protected:
    void resizeEvent(QResizeEvent* event) override;

public slots:
    void setImage(const std::vector<Vec3>& buffer, int width, int height);
    void setProgress(double percent);
    void setPixelColor(int x, int y, const Vec3& color);

signals:
    void imageSizeChanged(const QSize& newSize);

private:
    QLabel *imageLabel;
    QProgressBar *progressBar;
    QImage currentImage;

    void updateScaledImage();
};


#endif // RENDERINGWIDGET_H
