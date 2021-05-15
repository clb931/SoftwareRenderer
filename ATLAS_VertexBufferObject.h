#ifndef ATLAS_VBO_H
#include <cstdint>
#include <vector>
#include <memory>


namespace ATLAS {
	struct VertexBufferDescriptor {
		uint32_t elCount;
		uint32_t elSize;
		uint32_t offset;
		uint32_t stride;
	};

	struct VertexBufferObject {
		uint32_t size;
		void* data;
		std::vector<VertexBufferDescriptor> descriptors;
	};

	static void VboAlloc(VertexBufferObject* vbo, std::vector<VertexBufferDescriptor> descriptors) {
		vbo->size = 0;
		vbo->descriptors = descriptors;
		for (auto desc : descriptors) {
			vbo->size += desc.elCount * desc.elSize;			
		}
		vbo->data = new uint8_t[vbo->size];
	}

	static void VboFree(VertexBufferObject* vbo) {
		delete[] vbo->data;
	}

	static void VboPut(uint32_t descIndex, VertexBufferObject* vbo, uint32_t index, void* src) {
		VertexBufferDescriptor desc = vbo->descriptors.at(descIndex);
		void* dst = ((uint8_t*)vbo->data) + desc.offset + (index * (desc.stride + desc.elSize));
		std::memcpy(dst, src, desc.elSize);
	}

	static void* VboGet(uint32_t descIndex, VertexBufferObject* vbo, uint32_t index) {
		VertexBufferDescriptor desc = vbo->descriptors.at(descIndex);
		void* el = ((uint8_t*)vbo->data) + desc.offset + (index * (desc.stride + desc.elSize));
		return el;
	}

	static void VboFill(uint32_t descIndex, VertexBufferObject* vbo, void* src) {
		VertexBufferDescriptor desc = vbo->descriptors.at(descIndex);
		for (uint32_t i = 0; i < desc.elCount; ++i) {
			void* dst = ((uint8_t*)vbo->data) + desc.offset + (i * (desc.stride + desc.elSize));
			std::memcpy(dst, src, desc.elSize);
		}
	}

	static void VboFill(uint32_t descIndex, VertexBufferObject* vbo, void* srcData, VertexBufferDescriptor srcDesc) {
		VertexBufferDescriptor desc = vbo->descriptors.at(descIndex);
		for (uint32_t i = 0; i < desc.elCount; ++i) {
			void* dst = ((uint8_t*)vbo->data) + desc.offset + (i * (desc.stride + desc.elSize));
			void* src = ((uint8_t*)srcData) + srcDesc.offset + (i * (srcDesc.stride + srcDesc.elSize));
			std::memcpy(dst, src, desc.elSize);
		}
	}
}


#define ATLAS_VBO_H
#endif