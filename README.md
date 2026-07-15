# YtUICharts

> A native Slate-based chart widget plugin for Unreal Engine 5.7. No third-party dependencies, no WebView, no UMG widgets — all rendering is done directly through Slate draw elements.
>
> 一个基于原生 Slate 的 Unreal Engine 5.7 图表控件插件。无第三方依赖，无 WebView，无 UMG 控件 — 所有渲染均通过 Slate 绘图元素直接完成。

---

## Features / 特性

- **6 chart types / 6 种图表类型**: Line, Bar, Pie, Doughnut, Radar, Scatter
- **Native Slate rendering / 原生 Slate 渲染**: Uses `FSlateDrawElement::Make*` primitives — no external libraries / 使用 `FSlateDrawElement::Make*` 原语 — 无需外部库
- **Blueprint-friendly / 蓝图友好**: All configuration exposed to BP via `EditAnywhere, BlueprintReadWrite`
- **Multi-series support / 多系列支持**: Add multiple series with different colors / 支持添加不同颜色的多个系列
- **Automatic legend & axis / 自动图例和坐标轴**: Configurable legend position, grid, labels / 可配置的图例位置、网格、标签
- **Animation / 动画**: Smooth entry animation via `bAnimate` / `AnimSec`
- **Tooltip / 鼠标悬停提示**: Mouse hover shows coordinates (for Cartesian charts) / 鼠标悬停显示坐标（笛卡尔图表）

---

## Quick Start / 快速开始

### 1. Create the Widget / 创建控件

In Blueprint, create a widget that inherits from `UYtChartWidget`. Or use it directly in a `UUserWidget` subclass blueprint.

在蓝图中，创建一个继承自 `UYtChartWidget` 的控件。或者直接在 `UUserWidget` 子类蓝图中使用。

### 2. Set Data (Blueprint) / 设置数据（蓝图）

```cpp
// C++ example / C++ 示例
UYtChartWidget* Chart = ...;
Chart->Type = EYtChartType::Line;
Chart->Labels = { "A", "B", "C", "D", "E" };
Chart->SetData({ 10, 25, 15, 30, 20 }, "Series 1");
Chart->AddSeries("Series 2", { 8, 20, 12, 25, 18 }, FLinearColor::Red);
Chart->Refresh();
```

### 3. Set Data via API Call / 通过 API 调用设置数据

For dynamic data from HTTP responses / 用于 HTTP 响应的动态数据:

```cpp
void UMyChartWidget::OnHttpResponse(FString JsonString)
{
    TSharedPtr<FJsonObject> Json;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (FJsonSerializer::Deserialize(Reader, Json))
    {
        TArray<float> Values;
        for (auto& Elem : Json->GetArrayField("data"))
        {
            Values.Add(Elem->AsNumber());
        }
        SetData(Values, "API Data");
        Refresh();
    }
}
```

---

## Chart Types / 图表类型

| Type / 类型 | Description / 描述 |
|------|-------------|
| `Line` | Connected dots with optional smooth/step curves / 带可选平滑/阶梯曲线的连线图 |
| `Bar` | Vertical bars in Group / Stack / Percent layout / 分组/堆叠/百分比布局的垂直柱状图 |
| `Pie` | Full pie with optional percentage labels / 带可选百分比标签的饼图 |
| `Doughnut` | Ring chart with configurable hole size (`Style.PieHole`) / 带可调孔径的环形图 |
| `Radar` | Spider/star chart for multi-dimensional comparison / 用于多维比较的蜘蛛/星形图 |
| `Scatter` | Auto-paired point cloud from values / 从数值自动配对的散点图 |

---

## Public API / 公共 API

### Configuration Properties / 配置属性

```cpp
// Chart type (switch to change rendering) / 图表类型（切换以更改渲染）
UPROPERTY(EditAnywhere, BlueprintReadWrite) EYtChartType Type;

// Series data (Name, Values, Color, Thickness, bDots, bVisible) / 系列数据
UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FYtChartSeries> Series;

// Axis labels (shared across all Cartesian charts) / 坐标轴标签
UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> Labels;

// Style configuration (see FYtChartStyle below) / 样式配置
UPROPERTY(EditAnywhere, BlueprintReadWrite) FYtChartStyle Style;
```

### Blueprint Callable Functions / 蓝图可调用函数

```cpp
// Quick-set single series (replaces existing data) / 快速设置单个系列（替换现有数据）
UFUNCTION(BlueprintCallable) void SetData(const TArray<float>& Raw, FName Name = "Data");

// Append additional series / 追加额外系列
UFUNCTION(BlueprintCallable) void AddSeries(FName Name, const TArray<float>& Vals, FLinearColor Color);

// Clear all data / 清除所有数据
UFUNCTION(BlueprintCallable) void Clear();

// Force re-render (call after modifying Series/Labels/Style at runtime) / 强制重新渲染
UFUNCTION(BlueprintCallable) void Refresh();
```

---

## Types & Enums / 类型和枚举

### EYtChartType
```cpp
Line, Bar, Pie, Doughnut, Radar, Scatter
```

### EYtLegendPos
```cpp
Top, Bottom, Left, Right, Hide
```

### EYtBarLayout
```cpp
Group, Stack, Percent
```

### EYtLineCurve
```cpp
Straight, Smooth, Step
```

### FYtChartSeries
```cpp
FName Name;                    // Display name in legend / 图例显示名称
TArray<float> Values;          // Y-values (or single values for Pie) / Y 值
FLinearColor Color;            // Override auto-palette color / 覆盖自动调色板颜色
float Thickness = 2.0f;        // Line/bar border width / 线/柱边框宽度
bool bDots = true;             // Show data point markers / 显示数据点标记
bool bVisible = true;          // Toggle for legend filtering / 图例过滤开关
```

### FYtChartStyle
```cpp
// Canvas / 画布
FLinearColor Background;        // Chart background color / 图表背景色
FMargin Pad;                   // Inner padding / 内边距

// Title / 标题
bool bTitle;                   // Enable title display / 启用标题显示
FText Title;                   // Title text / 标题文本
int32 TitleFont = 16;          // Title font size / 标题字体大小
FLinearColor TitleColor;       // Title text color / 标题文本颜色

// Grid & Axis / 网格和坐标轴
bool bGrid = true;             // Show grid lines / 显示网格线
FLinearColor GridColor;        // Grid line color / 网格线颜色
FLinearColor AxisColor;        // Axis line color / 坐标轴颜色
bool bLabels = true;           // Show axis value labels / 显示坐标轴数值标签
int32 LabelFont = 10;          // Label font size / 标签字体大小
FLinearColor LabelColor;       // Label text color / 标签文本颜色

// Legend / 图例
bool bLegend = true;           // Show legend / 显示图例
EYtLegendPos LegendPos;        // Legend placement / 图例位置

// Tooltip / 鼠标悬停提示
bool bTooltip = true;          // Enable hover tooltip / 启用悬停提示

// Animation / 动画
bool bAnimate = true;          // Enable entry animation / 启用入场动画
float AnimSec = 0.6f;          // Animation duration / 动画持续时间

// Bar / 柱状图
EYtBarLayout BarStyle;         // Group / Stack / Percent / 分组/堆叠/百分比

// Line / 折线图
EYtLineCurve Curve;            // Straight / Smooth / Step / 直线/平滑/阶梯

// Pie / 饼图
float PieHole = 0.0f;          // 0 = Pie, >0 = Doughnut / 0 = 饼图，>0 = 环形图
bool bPct = true;              // Show percentage on slices / 在扇区上显示百分比

// Palette / 调色板（如果系列颜色为默认值则自动分配）
TArray<FLinearColor> Palette;
```

---

## Usage Examples / 使用示例

### Line Chart with Multiple Series / 多系列折线图
```cpp
Chart->Type = EYtChartType::Line;
Chart->Labels = { "Jan", "Feb", "Mar", "Apr", "May" };
Chart->SetData({ 12, 19, 15, 25, 22 }, "Revenue");
Chart->AddSeries("Cost", { 8, 12, 10, 15, 14 }, FLinearColor::Red);
Chart->Style.Curve = EYtLineCurve::Smooth;
Chart->Refresh();
```

### Bar Chart (Stacked) / 柱状图（堆叠）
```cpp
Chart->Type = EYtChartType::Bar;
Chart->Style.BarStyle = EYtBarLayout::Stack;
Chart->Clear();
Chart->AddSeries("Product A", { 10, 20, 30 }, FLinearColor(0.28, 0.50, 0.82));
Chart->AddSeries("Product B", { 15, 10, 25 }, FLinearColor(0.86, 0.32, 0.32));
Chart->Refresh();
```

### Pie / Doughnut / 饼图 / 环形图
```cpp
Chart->Type = EYtChartType::Doughnut;
Chart->Style.PieHole = 0.5f;  // 50% hole = doughnut / 50% 孔径 = 环形图
Chart->SetData({ 30, 25, 20, 15, 10 }, "Share");
Chart->Labels = { "A", "B", "C", "D", "E" };
Chart->Refresh();
```

### Radar Chart / 雷达图
```cpp
Chart->Type = EYtChartType::Radar;
Chart->Labels = { "Speed", "Power", "Agility", "Defense", "Magic" };
Chart->SetData({ 100, 80, 90, 70, 85 }, "Character A");
Chart->AddSeries("Character B", { 70, 95, 60, 85, 75 }, FLinearColor::Red);
Chart->Refresh();
```

---

## Layout Order / 绘制顺序

In `NativePaint`, elements are drawn in this order (each with `L++` to avoid overlap):

在 `NativePaint` 中，元素按以下顺序绘制（每次使用 `L++` 避免重叠）：

1. `DrawBg` — chart background / 图表背景
2. `DrawTitle` — chart title (top center) / 图表标题（顶部居中）
3. `DrawLegend` — legend (top/bottom/left/right/hide) / 图例（上/下/左/右/隐藏）
4. **Chart area / 图表区域** (computed from remaining space after Title/Legend/Pad) /（根据标题/图例/内边距后的剩余空间计算）:
   - Line/Bar/Scatter: `DrawGrid` + `DrawLine`/`DrawBar`/`DrawScatter`
   - Pie/Doughnut: `DrawPie` (triangle fan from center) / 从中心绘制的三角形扇形
   - Radar: grid rings + axis lines + `DrawRadar` (center-fan triangles) / 网格环 + 轴线和中心扇形三角形
5. `DrawTooltip` — mouse hover tooltip (topmost) / 鼠标悬停提示（最上层）

---

## Render Pipeline / 渲染管线

Each chart type splits its rendering into a separate `.cpp` file under `Source/YtUICharts/Private/`:

每种图表类型的渲染拆分到 `Source/YtUICharts/Private/` 下的独立 `.cpp` 文件中：

| File / 文件 | Responsibility / 职责 |
|------|---------------|
| `YtChartWidget.cpp` | Public API, `NativePaint`, `NativeTick`, `NativeOnMouseMove`, helpers, `DrawBg`, `DrawTitle`, `DrawLegend`, `DrawTooltip`, `DrawGrid` / 公共 API、NativePaint、NativeTick、NativeOnMouseMove、辅助函数、背景、标题、图例、提示、网格 |
| `YtChartRenderLine.cpp` | `DrawLine` — smooth/step/straight curves, dots, fill under line / 平滑/阶梯/直线曲线、数据点、线下填充 |
| `YtChartRenderBar.cpp` | `DrawBar` — grouped/stacked/percent layouts / 分组/堆叠/百分比布局 |
| `YtChartRenderPie.cpp` | `DrawPie` — triangle fan from center, doughnut mode / 从中心绘制的三角形扇形、环形模式 |
| `YtChartRenderRadar.cpp` | `DrawRadar` — outer rings, axis lines, center-fan triangles / 外环、轴线、中心扇形三角形 |
| `YtChartRenderScatter.cpp` | `DrawScatter` — auto-paired point rendering / 自动配对点渲染 |

All drawing uses `FSlateDrawElement::MakeBox`, `MakeLines`, `MakeText` — no third-party SDKs.

所有绘图均使用 `FSlateDrawElement::MakeBox`、`MakeLines`、`MakeText` — 无需第三方 SDK。

---

## Troubleshooting / 故障排除

### Radar / Pie fill not closing / 雷达图/饼图填充未闭合
Both charts use a **triangle fan** from the center point to the perimeter. The fill polygon must explicitly include the closing edge.

两种图表都使用从中心点到周边的**三角形扇形**。填充多边形必须显式包含闭合边。

```cpp
// Correct / 正确做法: explicit close for fill polygon / 填充多边形显式闭合
TArray<FVector2D> Poly = Pts;
Poly.Emplace(Poly[0]);  // Duplicate first point / 复制第一个点
FSlateDrawElement::MakeLines(..., Poly, ..., true, 0.0f);
```

### Tooltip not appearing / 鼠标悬停提示未出现
Ensure `Style.bTooltip = true` and the widget has mouse capture (default behavior). Tooltip only shows on Cartesian charts (Line, Bar, Scatter).

确保 `Style.bTooltip = true` 且控件已捕获鼠标（默认行为）。提示仅在笛卡尔图表（折线图、柱状图、散点图）上显示。

### Animation stuck at 0% / 动画卡在 0%
Call `Refresh()` after modifying data at runtime. `Refresh()` resets `AnimPct = 0` and `bAnimating = true`.

在运行时修改数据后调用 `Refresh()`。`Refresh()` 会将 `AnimPct` 重置为 0 并将 `bAnimating` 设为 true。

---
