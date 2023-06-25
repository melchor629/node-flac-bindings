export default async function setup() {
  // on several places, the native code does not load
  // workaround is to load it from here
  await import('../../lib/api.js')
}
