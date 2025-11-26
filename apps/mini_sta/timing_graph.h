/**
 * @file timing_graph.h
 * @brief Timing Graph Container - NetlistDB 的时序世界镜像
 * @details 管理所有 TimingNode 和 TimingArc 的容器，构建时序图的核心类
 */

#ifndef MINI_TIMING_GRAPH_H
#define MINI_TIMING_GRAPH_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "timing_path.h"

namespace mini {

// Forward declarations
class NetlistDB;
class Pin;
class Cell;
class Net;

/**
 * @class TimingGraph
 * @brief 时序图容器，构建并持有整个时序图
 * @details 相当于 NetlistDB 在时序世界的镜像，存储 Node 和 Arc 的完整层次结构
 */
class TimingGraph {
public:
    /**
     * @brief Constructor
     * @param netlist 指向底层物理网表数据库的指针（用于读取数据，但不修改）
     * @warning TimingGraph 知道 NetlistDB 的存在，但 NetlistDB 绝不包含 TimingGraph 的头文件，保证底层数据库的纯洁性
     */
    explicit TimingGraph(NetlistDB* netlist);

    /**
     * @brief Destructor
     */
    ~TimingGraph();

    // Disable copy, enable move to prevent accidental copying
    TimingGraph(const TimingGraph&) = delete;
    TimingGraph& operator=(const TimingGraph&) = delete;
    TimingGraph(TimingGraph&&) = default;
    TimingGraph& operator=(TimingGraph&&) = default;

    // ============================================================================
    // 核心方法：从 NetlistDB 构建时序图（最关键的设计）
    // ============================================================================

    /**
     * @brief 从物理网表构建时序图（工厂方法）
     * @details 最关键的构建逻辑，将物理连接转换为时序逻辑连接
     * 构建流程：
     *  1. 遍历 NetlistDB 中的所有 Cell
     *  2. 为每个 Cell 的每个 Pin 创建对应的 TimingNode
     *  3. 为每个 Net 创建 TimingArc (NET_ARC)：Driver Pin -> Load Pin
     *  4. 为每个 Cell 创建 TimingArc (CELL_ARC)：Input Pin -> Output Pin
     * @return true 构建成功，false 构建失败
     */
    bool buildFromNetlist();

    /**
     * @brief 清空所有节点和弧，重置图状态
     * @details 为后续重新构建图做准备
     */
    void clear();

    // ============================================================================
    // 节点管理（非拥有权访问）
    // ============================================================================

    /**
     * @brief 根据物理 pin 名称查找 TimingNode
     * @param pin_name 物理引脚名称
     * @return TimingNode 指针，未找到返回 nullptr
     */
    TimingNode* getNode(const std::string& pin_name) const;

    /**
     * @brief 根据物理 pin 指针查找 TimingNode
     * @param pin 物理引脚指针
     * @return TimingNode 指针，未找到返回 nullptr
     */
    TimingNode* getNode(const Pin* pin) const;

    /**
     * @brief 检查指定 pin 是否有时序节点
     * @param pin_name 引脚名称
     * @return true 存在，false 不存在
     */
    bool hasNode(const std::string& pin_name) const;

    /**
     * @brief 获取所有时序节点（所有权仍在 TimingGraph）
     * @return 节点容器的 const 引用
     */
    const std::vector<std::unique_ptr<TimingNode>>& getNodes() const { return nodes_; }

    /**
     * @brief 获取节点数量
     */
    size_t getNumNodes() const { return nodes_.size(); }

    // ============================================================================
    // 弧管理（非拥有权访问）
    // ============================================================================

    /**
     * @brief 获取所有时序弧（所有权仍在 TimingGraph）
     * @return 弧容器的 const 引用
     */
    const std::vector<std::unique_ptr<TimingArc>>& getArcs() const { return arcs_; }

    /**
     * @brief 获取弧数量
     */
    size_t getNumArcs() const { return arcs_.size(); }

    /**
     * @brief 获取从指定节点出发的所有弧
     * @param node 源节点
     * @return 出边弧的指针向量
     */
    std::vector<TimingArc*> getOutgoingArcs(const TimingNode* node) const;

    /**
     * @brief 获取指向指定节点的所有弧
     * @param node 目标节点
     * @return 入边弧的指针向量
     */
    std::vector<TimingArc*> getIncomingArcs(const TimingNode* node) const;

    // ============================================================================
    // 核心算法：拓扑排序（STA 必须按拓扑顺序传播）
    // ============================================================================

    /**
     * @brief 拓扑排序算法（DFS 实现）
     * @details STA 必须按拓扑顺序进行传播，确保数据一致性
     * 算法选择：DFS（深度优先搜索）实现，或 Kahn's 算法（BFS）均可
     * @return 按拓扑顺序排列的 TimingNode 指针向量
     * @note 如果图中存在环，返回空向量（时序图必须是 DAG）
     */
    std::vector<TimingNode*> topologicalSort() const;

    /**
     * @brief 使用 Kahn 算法进行拓扑排序（BFS 实现）
     * @details 备选实现方案，使用入度归零法
     * @return 按拓扑顺序排列的 TimingNode 指针向量
     */
    std::vector<TimingNode*> topologicalSortKahn() const;

    // ============================================================================
    // 核心算法：识别路径起点和终点
    // ============================================================================

    /**
     * @brief 获取所有时序路径的起点节点
     * @details 起点包括：
     *  - PI (Primary Input) 主输入端
     *  - DFF/Q（触发器输出端）
     * @return 起点节点的指针向量
     */
    std::vector<TimingNode*> getStartPoints() const;

    /**
     * @brief 获取所有时序路径的终点节点
     * @details 终点包括：
     *  - PO (Primary Output) 主输出端
     *  - DFF/D（触发器输入端）
     * @return 终点节点的指针向量
     */
    std::vector<TimingNode*> getEndPoints() const;

    // ============================================================================
    // 图拓扑查询
    // ============================================================================

    /**
     * @brief 获取所有主输入节点（无入边）
     * @return 主输入节点指针向量
     */
    std::vector<TimingNode*> getPrimaryInputs() const;

    /**
     * @brief 获取所有主输出节点（无出边）
     */
    std::vector<TimingNode*> getPrimaryOutputs() const;

    /**
     * @brief 获取所有时钟源节点
     */
    std::vector<TimingNode*> getClockSources() const;

    /**
     * @brief 验证图是否为合法的 DAG（有向无环图）
     * @return true 是无环图，false 存在环
     */
    bool isDAG() const;

    // ============================================================================
    // 时序数据管理和重置
    // ============================================================================

    /**
     * @brief 重置所有节点的时序数据（用于新一轮分析）
     * @details 调用每个 TimingNode::reset() 方法
     */
    void resetTimingData();

    /**
     * @brief 验证图的完整性
     */
    bool validate() const;

    // ============================================================================
    // 统计和调试
    // ============================================================================

    /**
     * @struct TimingGraphStats
     * @brief 时序图统计信息
     */
    struct TimingGraphStats {
        size_t num_nodes;           ///< 节点总数
        size_t num_arcs;            ///< 弧总数
        size_t num_pi;              ///< 主输入数
        size_t num_po;              ///< 主输出数
        size_t num_clock_nodes;     ///< 时钟节点数
    };

    /**
     * @brief 获取图统计信息
     */
    TimingGraphStats getStatistics() const;

    /**
     * @brief 打印统计信息到标准输出
     */
    void printStatistics() const;

    /**
     * @brief 打印图拓扑结构（用于调试）
     * @param verbose 是否打印详细信息
     */
    void printGraph(bool verbose = false) const;

private:
    // ============================================================================
    // 私有数据成员（所有权管理）
    // ============================================================================

    NetlistDB* netlist_;                                 ///< 底层物理网表数据库引用（只读）
    std::vector<std::unique_ptr<TimingNode>> nodes_;     ///< 拥有所有节点的内存所有权
    std::vector<std::unique_ptr<TimingArc>> arcs_;       ///< 拥有所有弧的内存所有权

    // 快速查找映射（非拥有权指针）
    std::unordered_map<std::string, TimingNode*> node_map_;         ///< Pin 名称 → TimingNode 映射
    std::unordered_map<const Pin*, TimingNode*> pin_to_node_map_;   ///< Pin 指针 → TimingNode 映射

    // ============================================================================
    // 私有辅助方法（buildFromNetlist 的子步骤）
    // ============================================================================

    /**
     * @brief 为物理引脚创建时序节点
     * @param pin 物理引脚指针
     * @return 创建的 TimingNode 指针
     * @details 被 buildFromNetlist() 调用，遍历所有 Cell 的 Pin
     */
    TimingNode* createNode(Pin* pin);

    /**
     * @brief 在两个节点之间创建时序弧
     * @param type 弧类型 (CELL_ARC 或 NET_ARC)
     * @param from_node 源节点
     * @param to_node 目标节点
     * @return 创建的 TimingArc 指针
     */
    TimingArc* createArc(TimingArcType type, TimingNode* from_node, TimingNode* to_node);

    /**
     * @brief 构建所有 CELL_ARC（遍历所有 Cell 的 Input → Output）
     * @return true 成功，false 失败
     */
    bool buildCellArcs();

    /**
     * @brief 构建所有 NET_ARC（遍历所有 Net 的 Driver → Load）
     * @return true 成功，false 失败
     */
    bool buildNetArcs();

    /**
     * @brief DFS 辅助函数（用于拓扑排序）
     * @param node 当前访问节点
     * @param visited 访问标记数组
     * @param order 拓扑顺序结果
     */
    void dfsTopologicalSort(TimingNode* node, std::vector<int>& visited,
                            std::vector<TimingNode*>& order) const;

    /**
     * @brief 获取节点入度（Kahn 算法辅助）
     */
    std::unordered_map<TimingNode*, int> calculateInDegrees() const;
};

} // namespace mini

#endif // MINI_TIMING_GRAPH_H
