#pragma once

#include "pb_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace pb {

class PbDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(PbDevice &pbDevice) : pbDevice{pbDevice} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<PbDescriptorSetLayout> build() const;

   private:
    PbDevice &pbDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  PbDescriptorSetLayout(
      PbDevice &pbDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~PbDescriptorSetLayout();
  PbDescriptorSetLayout(const PbDescriptorSetLayout &) = delete;
  PbDescriptorSetLayout &operator=(const PbDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  PbDevice &pbDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class PbDescriptorWriter;
};

class PbDescriptorPool {
 public:
  class Builder {
   public:
    Builder(PbDevice &pbDevice) : pbDevice{pbDevice} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<PbDescriptorPool> build() const;

   private:
    PbDevice &pbDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  PbDescriptorPool(
      PbDevice &pbDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~PbDescriptorPool();
  PbDescriptorPool(const PbDescriptorPool &) = delete;
  PbDescriptorPool &operator=(const PbDescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  PbDevice &pbDevice;
  VkDescriptorPool descriptorPool;

  friend class PbDescriptorWriter;
};

class PbDescriptorWriter {
 public:
  PbDescriptorWriter(PbDescriptorSetLayout &setLayout, PbDescriptorPool &pool);

  PbDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  PbDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  PbDescriptorSetLayout &setLayout;
  PbDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace pb