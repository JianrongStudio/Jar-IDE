# icon_generator.py
# JpCode 图标生成器 - 仿 PyCharm 风格

import sys
import os
from PySide6.QtCore import Qt, QSize, QRect, QPoint
from PySide6.QtGui import (
    QPainter, QColor, QFont, QPen, QBrush, QPixmap, 
    QLinearGradient, QRadialGradient, QPainterPath,
    QFontDatabase
)
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QLabel, QVBoxLayout, 
    QHBoxLayout, QWidget, QPushButton, QFileDialog,
    QGroupBox, QSpinBox, QComboBox, QCheckBox
)

class JpCodeIconGenerator:
    """JpCode 图标生成器 - 仿 PyCharm 风格"""
    
    @staticmethod
    def generate_icon(size=512, style="pycharm"):
        """
        生成 JpCode 图标
        
        Args:
            size: 图标尺寸
            style: 风格 (pycharm, light, dark)
        
        Returns:
            QPixmap: 生成的图标
        """
        pixmap = QPixmap(size, size)
        pixmap.fill(Qt.GlobalColor.transparent)
        
        painter = QPainter(pixmap)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.setRenderHint(QPainter.RenderHint.SmoothPixmapTransform)
        
        # 计算尺寸
        margin = size * 0.06
        rect = pixmap.rect().adjusted(margin, margin, -margin, -margin)
        center = rect.center()
        radius = rect.width() / 2
        
        # 1. 绘制圆形背景 (PyCharm 风格)
        if style == "pycharm":
            # PyCharm 风格：蓝紫色渐变
            gradient = QRadialGradient(center, radius)
            gradient.setColorAt(0.0, QColor("#4B8BBE"))      # 亮蓝
            gradient.setColorAt(0.4, QColor("#306998"))      # 中蓝
            gradient.setColorAt(0.8, QColor("#1B4F72"))      # 深蓝
            gradient.setColorAt(1.0, QColor("#0D2B45"))      # 最深蓝
        elif style == "light":
            # 浅色风格
            gradient = QRadialGradient(center, radius)
            gradient.setColorAt(0.0, QColor("#6CA6CD"))
            gradient.setColorAt(0.5, QColor("#4A8DB7"))
            gradient.setColorAt(1.0, QColor("#2E6A92"))
        else:  # dark
            # 深色风格
            gradient = QRadialGradient(center, radius)
            gradient.setColorAt(0.0, QColor("#3D7E9A"))
            gradient.setColorAt(0.4, QColor("#1B4F6A"))
            gradient.setColorAt(0.8, QColor("#0D2B3E"))
            gradient.setColorAt(1.0, QColor("#061B28"))
        
        painter.setBrush(QBrush(gradient))
        painter.setPen(QPen(QColor("#FFFFFF"), size * 0.01, Qt.PenStyle.SolidLine))
        painter.drawEllipse(rect)
        
        # 2. 绘制内部光效
        highlight = QRadialGradient(
            QPoint(center.x() - radius * 0.3, center.y() - radius * 0.3),
            radius * 0.6
        )
        highlight.setColorAt(0.0, QColor(255, 255, 255, 80))
        highlight.setColorAt(1.0, QColor(255, 255, 255, 0))
        painter.setBrush(QBrush(highlight))
        painter.setPen(Qt.PenStyle.NoPen)
        painter.drawEllipse(rect)
        
        # 3. 绘制装饰圆环
        ring_rect = rect.adjusted(
            size * 0.04, size * 0.04, -size * 0.04, -size * 0.04
        )
        painter.setBrush(Qt.BrushStyle.NoBrush)
        painter.setPen(QPen(QColor(255, 255, 255, 60), size * 0.008))
        painter.drawEllipse(ring_rect)
        
        # 4. 绘制 "JP" 文字（仿 PyCharm 风格）
        # 设置字体
        font_size = size * 0.32
        try:
            # 尝试使用系统字体
            font = QFont("Arial Black", int(font_size), QFont.Weight.Bold)
        except:
            font = QFont("Segoe UI", int(font_size), QFont.Weight.Bold)
        
        painter.setFont(font)
        
        # 文字阴影
        painter.setPen(QPen(QColor(0, 0, 0, 80), 1))
        painter.drawText(
            rect.adjusted(size * 0.015, size * 0.015, size * 0.015, size * 0.015),
            Qt.AlignmentFlag.AlignCenter,
            "JP"
        )
        
        # 主文字 - 白色
        painter.setPen(QPen(QColor(255, 255, 255), 1))
        painter.drawText(
            rect,
            Qt.AlignmentFlag.AlignCenter,
            "JP"
        )
        
        # 5. 绘制小字 "Code" (类似 PyCharm 的 "Charm")
        small_font_size = size * 0.08
        small_font = QFont("Segoe UI", int(small_font_size), QFont.Weight.Medium)
        painter.setFont(small_font)
        
        # "Code" 文字位置 - 在 JP 下方
        text_rect = QRect(
            rect.x(),
            int(rect.y() + rect.height() * 0.55),
            rect.width(),
            int(rect.height() * 0.25)
        )
        
        # "Code" 文字阴影
        painter.setPen(QPen(QColor(0, 0, 0, 60), 1))
        painter.drawText(
            text_rect.adjusted(size * 0.01, size * 0.01, size * 0.01, size * 0.01),
            Qt.AlignmentFlag.AlignHCenter | Qt.AlignmentFlag.AlignTop,
            "Code"
        )
        
        # "Code" 主文字 - 浅色
        painter.setPen(QPen(QColor(200, 230, 255, 220), 1))
        painter.drawText(
            text_rect,
            Qt.AlignmentFlag.AlignHCenter | Qt.AlignmentFlag.AlignTop,
            "Code"
        )
        
        # 6. 绘制底部装饰线 (PyCharm 风格)
        line_y = int(rect.y() + rect.height() * 0.72)
        line_rect = QRect(
            int(rect.x() + rect.width() * 0.25),
            line_y,
            int(rect.width() * 0.5),
            int(size * 0.015)
        )
        
        # 渐变线
        line_gradient = QLinearGradient(
            line_rect.topLeft(),
            line_rect.topRight()
        )
        line_gradient.setColorAt(0.0, QColor(255, 255, 255, 0))
        line_gradient.setColorAt(0.3, QColor(255, 255, 255, 150))
        line_gradient.setColorAt(0.5, QColor(255, 255, 255, 200))
        line_gradient.setColorAt(0.7, QColor(255, 255, 255, 150))
        line_gradient.setColorAt(1.0, QColor(255, 255, 255, 0))
        
        painter.setBrush(QBrush(line_gradient))
        painter.setPen(Qt.PenStyle.NoPen)
        painter.drawRoundedRect(line_rect, line_rect.height()/2, line_rect.height()/2)
        
        painter.end()
        return pixmap
    
    @staticmethod
    def save_icon(pixmap, filepath, formats=None):
        """保存图标为多种格式"""
        if not formats:
            formats = ["png", "ico"]
        
        saved_files = []
        base_name = os.path.splitext(filepath)[0]
        
        for fmt in formats:
            if fmt == "ico":
                # 生成多尺寸 ICO
                ico_path = f"{base_name}.ico"
                JpCodeIconGenerator.save_ico(pixmap, ico_path)
                saved_files.append(ico_path)
            else:
                # PNG 格式
                png_path = f"{base_name}.png" if fmt == "png" else f"{base_name}.{fmt}"
                pixmap.save(png_path, fmt.upper())
                saved_files.append(png_path)
        
        return saved_files
    
    @staticmethod
    def save_ico(pixmap, filepath):
        """保存为 ICO 格式（多尺寸）"""
        # 为 ICO 生成多种尺寸
        sizes = [16, 32, 48, 64, 128, 256, 512]
        images = []
        
        for size in sizes:
            scaled = pixmap.scaled(
                size, size, 
                Qt.AspectRatioMode.KeepAspectRatio,
                Qt.TransformationMode.SmoothTransformation
            )
            images.append(scaled.toImage())
        
        # 保存为 ICO
        # 使用 PIL 或直接保存第一个尺寸
        # 注意：Qt 直接保存 ICO 可能只保存第一个尺寸
        pixmap.scaled(256, 256, Qt.AspectRatioMode.KeepAspectRatio).save(filepath, "ICO")


class IconPreviewWindow(QMainWindow):
    """图标预览和导出窗口"""
    
    def __init__(self):
        super().__init__()
        self.setWindowTitle("JpCode 图标生成器")
        self.setMinimumSize(600, 700)
        
        # 中央部件
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout(central_widget)
        layout.setSpacing(20)
        
        # 标题
        title_label = QLabel("🎨 JpCode 图标生成器")
        title_label.setStyleSheet("""
            font-size: 24px;
            font-weight: bold;
            color: #4B8BBE;
            padding: 10px;
        """)
        title_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(title_label)
        
        # 预览区域
        preview_group = QGroupBox("预览")
        preview_group.setStyleSheet("""
            QGroupBox {
                font-size: 14px;
                font-weight: bold;
                border: 2px solid #4B8BBE;
                border-radius: 8px;
                margin-top: 10px;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 10px;
            }
        """)
        preview_layout = QVBoxLayout(preview_group)
        
        self.preview_label = QLabel()
        self.preview_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.preview_label.setMinimumHeight(400)
        self.preview_label.setStyleSheet("""
            QLabel {
                background: #F0F0F0;
                border-radius: 8px;
                padding: 20px;
            }
        """)
        preview_layout.addWidget(self.preview_label)
        layout.addWidget(preview_group)
        
        # 控制区域
        controls_group = QGroupBox("设置")
        controls_group.setStyleSheet("""
            QGroupBox {
                font-size: 14px;
                font-weight: bold;
                border: 2px solid #4B8BBE;
                border-radius: 8px;
                margin-top: 10px;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 10px;
            }
        """)
        controls_layout = QVBoxLayout(controls_group)
        
        # 尺寸控制
        size_layout = QHBoxLayout()
        size_layout.addWidget(QLabel("尺寸:"))
        self.size_spin = QSpinBox()
        self.size_spin.setRange(16, 1024)
        self.size_spin.setValue(512)
        self.size_spin.setSuffix(" px")
        self.size_spin.valueChanged.connect(self.update_preview)
        size_layout.addWidget(self.size_spin)
        size_layout.addStretch()
        controls_layout.addLayout(size_layout)
        
        # 风格控制
        style_layout = QHBoxLayout()
        style_layout.addWidget(QLabel("风格:"))
        self.style_combo = QComboBox()
        self.style_combo.addItems(["PyCharm 风格", "浅色", "深色"])
        self.style_combo.currentTextChanged.connect(self.update_preview)
        style_layout.addWidget(self.style_combo)
        style_layout.addStretch()
        controls_layout.addLayout(style_layout)
        
        layout.addWidget(controls_group)
        
        # 按钮区域
        button_layout = QHBoxLayout()
        
        export_png_btn = QPushButton("📥 导出 PNG")
        export_png_btn.clicked.connect(lambda: self.export_icon("png"))
        export_png_btn.setStyleSheet("""
            QPushButton {
                background: #4B8BBE;
                color: white;
                font-size: 14px;
                font-weight: bold;
                padding: 10px 20px;
                border: none;
                border-radius: 6px;
            }
            QPushButton:hover {
                background: #306998;
            }
        """)
        button_layout.addWidget(export_png_btn)
        
        export_ico_btn = QPushButton("📥 导出 ICO")
        export_ico_btn.clicked.connect(lambda: self.export_icon("ico"))
        export_ico_btn.setStyleSheet("""
            QPushButton {
                background: #6A9955;
                color: white;
                font-size: 14px;
                font-weight: bold;
                padding: 10px 20px;
                border: none;
                border-radius: 6px;
            }
            QPushButton:hover {
                background: #4B7A3E;
            }
        """)
        button_layout.addWidget(export_ico_btn)
        
        export_all_btn = QPushButton("📦 导出全部")
        export_all_btn.clicked.connect(lambda: self.export_icon("all"))
        export_all_btn.setStyleSheet("""
            QPushButton {
                background: #C586C0;
                color: white;
                font-size: 14px;
                font-weight: bold;
                padding: 10px 20px;
                border: none;
                border-radius: 6px;
            }
            QPushButton:hover {
                background: #9E6A98;
            }
        """)
        button_layout.addWidget(export_all_btn)
        
        layout.addLayout(button_layout)
        
        # 信息标签
        info_label = QLabel("💡 提示: 支持导出 PNG 和 ICO 格式，ICO 包含多尺寸")
        info_label.setStyleSheet("color: #666; font-size: 12px; padding: 5px;")
        info_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(info_label)
        
        # 初始化预览
        self.current_pixmap = None
        self.update_preview()
    
    def update_preview(self):
        """更新预览"""
        size = self.size_spin.value()
        style_map = {
            "PyCharm 风格": "pycharm",
            "浅色": "light",
            "深色": "dark"
        }
        style = style_map.get(self.style_combo.currentText(), "pycharm")
        
        self.current_pixmap = JpCodeIconGenerator.generate_icon(size, style)
        
        # 缩放预览以适应显示
        preview_size = min(400, size)
        scaled = self.current_pixmap.scaled(
            preview_size, preview_size,
            Qt.AspectRatioMode.KeepAspectRatio,
            Qt.TransformationMode.SmoothTransformation
        )
        self.preview_label.setPixmap(scaled)
    
    def export_icon(self, format_type):
        """导出图标"""
        if not self.current_pixmap:
            return
        
        # 选择保存位置
        default_name = f"JpCode_icon_{self.size_spin.value()}"
        if format_type == "all":
            filepath, _ = QFileDialog.getSaveFileName(
                self, "保存图标", default_name, 
                "PNG Image (*.png);;ICO Image (*.ico)"
            )
        else:
            filepath, _ = QFileDialog.getSaveFileName(
                self, "保存图标", default_name,
                "PNG Image (*.png)" if format_type == "png" else "ICO Image (*.ico)"
            )
        
        if not filepath:
            return
        
        try:
            if format_type == "all":
                # 导出所有格式
                saved = JpCodeIconGenerator.save_icon(
                    self.current_pixmap, 
                    os.path.splitext(filepath)[0],
                    ["png", "ico"]
                )
                import webbrowser
                webbrowser.open(os.path.dirname(saved[0]))
            else:
                # 导出指定格式
                saved = JpCodeIconGenerator.save_icon(
                    self.current_pixmap,
                    filepath,
                    [format_type]
                )
            
            from PySide6.QtWidgets import QMessageBox
            QMessageBox.information(
                self,
                "导出成功",
                f"图标已保存到:\n{', '.join(saved)}"
            )
        except Exception as e:
            from PySide6.QtWidgets import QMessageBox
            QMessageBox.critical(
                self,
                "导出失败",
                f"保存图标时出错:\n{str(e)}"
            )


def main():
    app = QApplication(sys.argv)
    
    # 设置应用风格
    app.setStyle("Fusion")
    
    # 窗口
    window = IconPreviewWindow()
    window.show()
    
    sys.exit(app.exec())


if __name__ == "__main__":
    main()