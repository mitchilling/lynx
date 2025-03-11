export class TextEncoder {
  constructor() {}

  encode(str: string): ArrayBuffer {
    return new Uint8Array(globalThis.TextCodecHelper.encode(str));
  }

  encodeInto() {
    throw TypeError('TextEncoder().encodeInto not supported');
  }

  get encoding() {
    return 'utf-8';
  }
}
